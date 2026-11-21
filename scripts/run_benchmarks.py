#!/usr/bin/env python3
"""Run performance benchmarks for tasks under benchmarks/<category>/<task>/."""

import argparse
import json
import math
import resource
import statistics
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path
from typing import Any

REPO_ROOT = Path(__file__).resolve().parent.parent
BENCHMARKS_DIR = REPO_ROOT / "benchmarks"
DEFAULT_RUNS = 5
DEFAULT_TIMEOUT_SEC = 10.0

# Omit -Werror/-Wconversion/-Wshadow so third-party baseline solutions compile cleanly.
COMPILE_CMD = [
    "g++",
    "-std=c++20",
    "-march=native",
    "-O2",
    "-Wall",
    "-Wextra",
    "-I",
    str(REPO_ROOT),
]


@dataclass(frozen=True)
class CaseSpec:
    name: str
    generator: Path
    generator_rel: str
    args: tuple[str, ...]


@dataclass(frozen=True)
class RunOutput:
    skipped: bool
    skip_reason: str
    answer_hash: str | None
    timings_sec: dict[str, float]


@dataclass(frozen=True)
class CaseSolResult:
    status: str  # "OK", "SKIP", or an error status
    detail: str
    answer_hash: str | None
    medians_sec: dict[str, float]


def parse_solution_output(stdout: str) -> RunOutput:
    """Parse structured benchmark output from a solution's stdout."""
    skipped = False
    skip_reason = ""
    answer_hash: str | None = None
    timings_sec: dict[str, float] = {}

    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if not line:
            continue

        parts = line.split()
        cmd = parts[0]

        if cmd == "skip":
            if skipped or answer_hash is not None or timings_sec:
                raise ValueError("Invalid output: 'skip' cannot be combined with other directives")
            skipped = True
            skip_reason = line[len("skip"):].strip()
        elif cmd == "hash":
            if skipped:
                raise ValueError("Invalid output: 'hash' cannot appear after 'skip'")
            if len(parts) != 2:
                raise ValueError(f"Invalid 'hash' line: {line!r}")
            if answer_hash is not None:
                raise ValueError("Duplicate 'hash' line")
            hash_str = parts[1]
            if not hash_str.isascii() or not hash_str.isdigit():
                raise ValueError(f"Invalid 'hash' value (expected 64-bit unsigned integer): {hash_str!r}")
            hash_int = int(hash_str)
            if hash_int >= (1 << 64) or hash_str != str(hash_int):
                raise ValueError(f"Invalid 'hash' value (expected canonical 64-bit unsigned integer): {hash_str!r}")
            answer_hash = hash_str
        elif cmd == "time":
            if skipped:
                raise ValueError("Invalid output: 'time' cannot appear after 'skip'")
            if len(parts) != 3:
                raise ValueError(f"Invalid 'time' line: {line!r}")
            section, value_str = parts[1], parts[2]
            if section in timings_sec:
                raise ValueError(f"Duplicate timing section: {section!r}")
            try:
                value = float(value_str)
            except ValueError as exc:
                raise ValueError(f"Invalid timing value {value_str!r} for section {section!r}") from exc
            if not math.isfinite(value) or value < 0.0:
                raise ValueError(f"Timing value must be finite and non-negative, got {value}")
            timings_sec[section] = value
        else:
            raise ValueError(f"Unexpected output line: {line!r}")

    if skipped:
        return RunOutput(skipped=True, skip_reason=skip_reason, answer_hash=None, timings_sec={})

    if answer_hash is None:
        raise ValueError("Missing required 'hash' line")
    if not timings_sec:
        raise ValueError("Missing required 'time' line")

    return RunOutput(skipped=False, skip_reason="", answer_hash=answer_hash, timings_sec=timings_sec)


def compile_cpp(source: Path, output: Path) -> None:
    """Compile a C++ source file and raise RuntimeError on failure."""
    cmd = COMPILE_CMD + [str(source), "-o", str(output)]
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.returncode != 0:
        details = res.stderr.strip() or res.stdout.strip()
        raise RuntimeError(f"Compilation failed for {source}:\n{details}")


def load_task_config(
    task_dir: Path,
    runs_override: int | None,
    timeout_override: float | None,
) -> tuple[int, float, list[CaseSpec]]:
    """Load and validate task.json in task_dir."""
    config_path = task_dir / "task.json"
    with config_path.open("r", encoding="utf-8") as f:
        raw = json.load(f)

    if not isinstance(raw, dict):
        raise ValueError(f"{config_path}: root must be a JSON object")

    runs = runs_override if runs_override is not None else raw.get("runs", DEFAULT_RUNS)
    if not isinstance(runs, int) or isinstance(runs, bool) or runs < 1:
        raise ValueError(f"{config_path}: 'runs' must be an integer >= 1")

    timeout_sec = timeout_override if timeout_override is not None else raw.get("timeout_sec", DEFAULT_TIMEOUT_SEC)
    if not isinstance(timeout_sec, (int, float)) or isinstance(timeout_sec, bool) or timeout_sec <= 0:
        raise ValueError(f"{config_path}: 'timeout_sec' must be a positive number")

    raw_cases = raw.get("cases")
    if not isinstance(raw_cases, list) or not raw_cases:
        raise ValueError(f"{config_path}: 'cases' must be a non-empty list")

    generators_dir = (task_dir / "generators").resolve()
    if not generators_dir.is_dir():
        raise ValueError(f"{task_dir}: missing 'generators' directory")

    cases: list[CaseSpec] = []
    seen_names: set[str] = set()
    for idx, entry in enumerate(raw_cases):
        if not isinstance(entry, dict):
            raise ValueError(f"{config_path}: case #{idx} must be an object")

        gen_rel = entry.get("generator")
        if not isinstance(gen_rel, str) or not gen_rel:
            raise ValueError(f"{config_path}: case #{idx} missing 'generator'")

        gen_path = (generators_dir / gen_rel).resolve()
        try:
            gen_path.relative_to(generators_dir)
        except ValueError as exc:
            raise ValueError(f"{config_path}: generator path escapes generators/: {gen_rel}") from exc
        if not gen_path.is_file():
            raise ValueError(f"{config_path}: generator not found: {gen_path}")
        if gen_path.suffix != ".cpp":
            raise ValueError(f"{config_path}: generator must be a .cpp file: {gen_path.name}")

        raw_args = entry.get("args", [])
        if not isinstance(raw_args, list):
            raise ValueError(f"{config_path}: case #{idx} 'args' must be a list")
        args_tuple = tuple(str(a) for a in raw_args)

        name = entry.get("name")
        if name is None:
            suffix = "_" + "_".join(args_tuple) if args_tuple else ""
            name = f"{gen_path.stem}{suffix}"
        if not isinstance(name, str) or not name.strip():
            raise ValueError(f"{config_path}: case #{idx} 'name' must be a non-empty string")
        if name in seen_names:
            raise ValueError(f"{config_path}: duplicate case name: {name!r}")
        seen_names.add(name)

        cases.append(CaseSpec(name=name, generator=gen_path, generator_rel=gen_rel, args=args_tuple))

    return runs, float(timeout_sec), cases


def unlimit_stack() -> None:
    """Raise stack size limit to the hard limit for deep recursion in solutions."""
    soft, hard = resource.getrlimit(resource.RLIMIT_STACK)
    if soft != hard:
        resource.setrlimit(resource.RLIMIT_STACK, (hard, hard))


def generate_cases(
    cases: list[CaseSpec],
    bin_dir: Path,
    cases_dir: Path,
    temp_cwd: Path,
    timeout_sec: float,
) -> dict[str, Path]:
    """Compile generators once and generate all test case files into cases_dir."""
    compiled_gens: dict[Path, Path] = {}
    for case in cases:
        if case.generator in compiled_gens:
            continue
        exe_path = bin_dir / f"gen_{len(compiled_gens)}_{case.generator.stem}.out"
        compile_cpp(case.generator, exe_path)
        compiled_gens[case.generator] = exe_path

    case_files: dict[str, Path] = {}
    for idx, case in enumerate(cases):
        out_path = cases_dir / f"case_{idx:03d}.in"
        cmd = [str(compiled_gens[case.generator])] + list(case.args)
        try:
            with out_path.open("wb") as out_f:
                res = subprocess.run(
                    cmd,
                    stdout=out_f,
                    stderr=subprocess.PIPE,
                    cwd=temp_cwd,
                    timeout=timeout_sec,
                    preexec_fn=unlimit_stack,
                )
        except subprocess.TimeoutExpired as exc:
            raise RuntimeError(f"Generator timed out for case {case.name!r} after {timeout_sec}s") from exc

        if res.returncode != 0:
            err = res.stderr.decode("utf-8", errors="replace").strip()
            raise RuntimeError(
                f"Generator failed for case {case.name!r} (exit code {res.returncode}):\n{err}"
            )
        case_files[case.name] = out_path

    return case_files


def run_solution_on_case(
    exe_path: Path,
    input_path: Path,
    runs: int,
    timeout_sec: float,
) -> CaseSolResult:
    """Run a compiled solution multiple times on one test case and return median timings."""
    expected_hash: str | None = None
    expected_sections: list[str] | None = None
    section_samples: dict[str, list[float]] = {}
    total_samples: list[float] = []

    for run_idx in range(runs):
        try:
            with input_path.open("rb") as in_f:
                res = subprocess.run(
                    [str(exe_path)],
                    stdin=in_f,
                    capture_output=True,
                    text=True,
                    timeout=timeout_sec,
                    preexec_fn=unlimit_stack,
                )
        except subprocess.TimeoutExpired:
            return CaseSolResult("TLE", f">{timeout_sec:g}s on run {run_idx + 1}", None, {})

        if res.returncode != 0:
            err = res.stderr.strip()
            detail = f"exit {res.returncode}" + (f": {err.splitlines()[0]}" if err else "")
            return CaseSolResult("RE", detail, None, {})

        try:
            parsed = parse_solution_output(res.stdout)
        except ValueError as exc:
            return CaseSolResult("INVALID", str(exc), None, {})

        if parsed.skipped:
            return CaseSolResult("SKIP", parsed.skip_reason, None, {})

        if expected_hash is None:
            expected_hash = parsed.answer_hash
        elif parsed.answer_hash != expected_hash:
            return CaseSolResult(
                "HASH_ERROR",
                f"nondeterministic hash: {expected_hash} vs {parsed.answer_hash}",
                None,
                {},
            )

        sections = list(parsed.timings_sec.keys())
        if expected_sections is None:
            expected_sections = sections
            section_samples = {sec: [] for sec in sections}
        elif sections != expected_sections:
            return CaseSolResult(
                "INVALID",
                f"inconsistent timing sections across runs: {expected_sections} vs {sections}",
                None,
                {},
            )

        for sec, val in parsed.timings_sec.items():
            section_samples[sec].append(val)
        total_samples.append(sum(parsed.timings_sec.values()))

    medians = {sec: statistics.median(vals) for sec, vals in section_samples.items()}
    if "total" not in medians:
        medians["total"] = statistics.median(total_samples)
    return CaseSolResult("OK", "", expected_hash, medians)


def build_case_report(
    case: CaseSpec,
    sol_names: list[str],
    results: dict[str, CaseSolResult],
) -> tuple[dict[str, Any], bool]:
    """Build one test case's JSON report and verify answer hash consistency."""
    observed_hashes = {
        sol: res.answer_hash
        for sol, res in results.items()
        if res.status == "OK" and res.answer_hash is not None
    }
    unique_hashes = set(observed_hashes.values())
    hash_ok = len(unique_hashes) <= 1
    case_hash = next(iter(unique_hashes)) if observed_hashes and hash_ok else None

    solutions_report: dict[str, dict[str, Any]] = {}
    for sol in sol_names:
        res = results[sol]
        if res.status == "OK":
            if not hash_ok:
                solutions_report[sol] = {
                    "status": "HASH_ERROR",
                    "error": f"hash mismatch across solutions (got {res.answer_hash})",
                }
            else:
                solutions_report[sol] = {
                    "status": "OK",
                    "hash": res.answer_hash,
                    "medians_sec": {sec: round(val, 9) for sec, val in res.medians_sec.items()},
                }
        elif res.status == "SKIP":
            entry: dict[str, Any] = {"status": "SKIP"}
            if res.detail:
                entry["reason"] = res.detail
            solutions_report[sol] = entry
        else:
            solutions_report[sol] = {
                "status": res.status,
                "error": res.detail,
            }

    all_runs_ok = all(res.status in {"OK", "SKIP"} for res in results.values())
    report: dict[str, Any] = {
        "name": case.name,
        "generator": case.generator_rel,
        "args": list(case.args),
        "hash": case_hash,
        "solutions": solutions_report,
    }
    return report, (hash_ok and all_runs_ok)


def discover_tasks(targets: list[str]) -> list[Path]:
    """Resolve target paths into sorted unique task directories containing task.json."""
    search_roots = [Path(t) for t in targets] if targets else [BENCHMARKS_DIR]
    tasks: list[Path] = []
    seen: set[Path] = set()

    for root in search_roots:
        if not root.exists():
            raise FileNotFoundError(f"Target path not found: {root}")
        if not root.is_dir():
            raise ValueError(f"Expected a directory, got: {root}")
        resolved = root.resolve()
        if (resolved / "task.json").is_file():
            if resolved not in seen:
                seen.add(resolved)
                tasks.append(resolved)
        else:
            for cfg in sorted(resolved.rglob("task.json")):
                task_dir = cfg.parent
                if task_dir not in seen:
                    seen.add(task_dir)
                    tasks.append(task_dir)

    return sorted(tasks)


def format_task_path(task_dir: Path) -> str:
    try:
        return task_dir.relative_to(REPO_ROOT).as_posix()
    except ValueError:
        return str(task_dir)


def run_task(
    task_dir: Path,
    runs_override: int | None,
    timeout_override: float | None,
    case_filters: set[str],
    sol_filters: set[str],
) -> tuple[dict[str, Any], bool]:
    """Compile, generate inputs, execute benchmarks, and return a JSON report for one task."""
    rel_task = format_task_path(task_dir)

    runs, timeout_sec, cases = load_task_config(task_dir, runs_override, timeout_override)
    if case_filters:
        cases = [c for c in cases if c.name in case_filters]
        if not cases:
            raise ValueError(f"{rel_task}: no cases matched filter {sorted(case_filters)}")

    solutions_dir = task_dir / "solutions"
    if not solutions_dir.is_dir():
        raise ValueError(f"{rel_task}: missing 'solutions' directory")

    sol_sources = sorted(solutions_dir.rglob("*.cpp"))
    if not sol_sources:
        raise ValueError(f"{rel_task}: no .cpp files found in solutions/")

    solutions: list[tuple[str, Path]] = []
    for src in sol_sources:
        sol_name = src.relative_to(solutions_dir).with_suffix("").as_posix()
        if not sol_filters or sol_name in sol_filters or src.name in sol_filters:
            solutions.append((sol_name, src))

    if not solutions:
        raise ValueError(f"{rel_task}: no solutions matched filter {sorted(sol_filters)}")

    task_ok = True
    case_reports: list[dict[str, Any]] = []
    with tempfile.TemporaryDirectory(prefix="library3372_bench_") as tmp:
        tmp_path = Path(tmp)
        bin_dir = tmp_path / "bin"
        cases_dir = tmp_path / "cases"
        bin_dir.mkdir()
        cases_dir.mkdir()

        case_files = generate_cases(cases, bin_dir, cases_dir, tmp_path, timeout_sec)

        sol_bins: dict[str, Path] = {}
        for idx, (sol_name, src) in enumerate(solutions):
            exe_path = bin_dir / f"sol_{idx}_{src.stem}.out"
            compile_cpp(src, exe_path)
            sol_bins[sol_name] = exe_path

        sol_names = [name for name, _ in solutions]
        for case in cases:
            case_results: dict[str, CaseSolResult] = {}
            for sol_name in sol_names:
                case_results[sol_name] = run_solution_on_case(
                    sol_bins[sol_name],
                    case_files[case.name],
                    runs,
                    timeout_sec,
                )
            case_report, case_ok = build_case_report(case, sol_names, case_results)
            case_reports.append(case_report)
            if not case_ok:
                task_ok = False

    task_report: dict[str, Any] = {
        "task": rel_task,
        "runs": runs,
        "timeout_sec": timeout_sec,
        "cases": case_reports,
    }
    return task_report, task_ok


def main() -> int:
    parser = argparse.ArgumentParser(description="Run competitive-programming style benchmarks.")
    parser.add_argument(
        "targets",
        nargs="*",
        help="Task or category directories (default: benchmarks/).",
    )
    parser.add_argument("--runs", type=int, default=None, help="Override repetitions per case.")
    parser.add_argument("--timeout", type=float, default=None, help="Override timeout in seconds.")
    parser.add_argument("-c", "--case", action="append", default=[], help="Filter by case name.")
    parser.add_argument("-s", "--solution", action="append", default=[], help="Filter by solution name.")
    args = parser.parse_args()

    if args.runs is not None and args.runs < 1:
        print("Error: --runs must be >= 1", file=sys.stderr)
        return 1
    if args.timeout is not None and args.timeout <= 0:
        print("Error: --timeout must be > 0", file=sys.stderr)
        return 1

    try:
        tasks = discover_tasks(args.targets)
    except (FileNotFoundError, ValueError) as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1

    if not tasks:
        if args.targets:
            print("Error: No task.json found under specified target(s).", file=sys.stderr)
            return 1
        print(json.dumps({"ok": True, "tasks": []}, indent=2))
        return 0

    all_ok = True
    task_reports: list[dict[str, Any]] = []
    for task_dir in tasks:
        try:
            task_report, ok = run_task(task_dir, args.runs, args.timeout, set(args.case), set(args.solution))
            task_reports.append(task_report)
            if not ok:
                all_ok = False
        except Exception as exc:
            task_reports.append({
                "task": format_task_path(task_dir),
                "error": str(exc),
            })
            all_ok = False

    print(json.dumps({"ok": all_ok, "tasks": task_reports}, indent=2))
    return 0 if all_ok else 1


if __name__ == "__main__":
    sys.exit(main())
