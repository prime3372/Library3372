#!/usr/bin/env python3
"""Compile and run unit tests under tests/."""

import argparse
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
import resource
import subprocess
import sys
import tempfile
import threading

REPO_ROOT = Path(__file__).resolve().parent.parent
TESTS_DIR = REPO_ROOT / "tests"
MAX_WORKERS = 16
DEFAULT_TIMEOUT_SEC = 2.0
DEFAULT_SANITIZE_TIMEOUT_SEC = 5.0

BASE_COMPILE_CMD = [
    "g++",
    "-std=c++20",
    "-march=native",
    "-Wall",
    "-Wextra",
    "-Wconversion",
    "-Wshadow",
    "-Werror",
    "-I",
    str(REPO_ROOT),
]

NORMAL_FLAGS = ["-O2"]
SANITIZE_FLAGS = [
    "-O1",
    "-g",
    "-fsanitize=address,undefined",
    "-fno-sanitize-recover=all",
    "-fno-omit-frame-pointer",
]

abort_event = threading.Event()
print_lock = threading.Lock()


def write_error_log(
    file_path: Path, stage: str, message: str, stdout: str, stderr: str
) -> None:
    with print_lock:
        print(f"File: {file_path}")
        print(f"Stage: {stage}")
        print(f"Error: {message}")
        print("-" * 40)
        print(f"[STDOUT]\n{stdout}")
        print("-" * 40)
        print(f"[STDERR]\n{stderr}")
        print("-" * 40 + "\n")


def unlimit_stack() -> None:
    """Raise stack size limit to the hard limit for deep recursion in tests."""
    soft, hard = resource.getrlimit(resource.RLIMIT_STACK)
    if soft != hard:
        resource.setrlimit(resource.RLIMIT_STACK, (hard, hard))


def discover_test_files(targets: list[str]) -> list[Path]:
    """Collect .cpp test files from targets or TESTS_DIR."""
    if not targets:
        if not TESTS_DIR.is_dir():
            raise FileNotFoundError(f"Directory '{TESTS_DIR}' not found.")
        return sorted(TESTS_DIR.rglob("*.cpp"))

    found: list[Path] = []
    for raw_target in targets:
        path = Path(raw_target)
        if not path.is_absolute():
            path = (Path.cwd() / path).resolve()
        if not path.exists():
            raise FileNotFoundError(f"Target '{raw_target}' not found.")
        if path.is_dir():
            found.extend(sorted(path.rglob("*.cpp")))
        elif path.is_file():
            if path.suffix != ".cpp":
                raise ValueError(f"Target '{raw_target}' is not a .cpp file.")
            found.append(path)
        else:
            raise ValueError(f"Target '{raw_target}' is neither a file nor a directory.")

    return sorted(dict.fromkeys(found))


def compile_and_run(
    idx: int,
    cpp_file: Path,
    bin_dir: Path,
    sanitize: bool,
    timeout_sec: float,
) -> bool:
    if abort_event.is_set():
        return False

    out_exe = bin_dir / f"test_{idx}_{cpp_file.stem}.out"
    flags = SANITIZE_FLAGS if sanitize else NORMAL_FLAGS
    compile_args = BASE_COMPILE_CMD + flags + [str(cpp_file), "-o", str(out_exe)]

    try:
        comp_res = subprocess.run(compile_args, capture_output=True, text=True)
        if comp_res.returncode != 0:
            abort_event.set()
            write_error_log(
                cpp_file,
                "Compilation",
                f"Exit code {comp_res.returncode}",
                comp_res.stdout,
                comp_res.stderr,
            )
            return False
    except Exception as exc:
        abort_event.set()
        write_error_log(cpp_file, "Compilation", str(exc), "", "")
        return False

    if abort_event.is_set():
        return False

    try:
        run_res = subprocess.run(
            [str(out_exe)],
            capture_output=True,
            text=True,
            timeout=timeout_sec,
            preexec_fn=unlimit_stack,
        )
        if run_res.returncode != 0:
            abort_event.set()
            write_error_log(
                cpp_file,
                "Execution",
                f"Runtime Error (Exit code {run_res.returncode})",
                run_res.stdout,
                run_res.stderr,
            )
            return False
    except subprocess.TimeoutExpired as exc:
        abort_event.set()
        stdout = exc.stdout.decode(errors="replace") if exc.stdout else ""
        stderr = exc.stderr.decode(errors="replace") if exc.stderr else ""
        write_error_log(
            cpp_file,
            "Execution",
            f"Timeout ({timeout_sec:g}s exceeded)",
            stdout,
            stderr,
        )
        return False
    except Exception as exc:
        abort_event.set()
        write_error_log(cpp_file, "Execution", str(exc), "", "")
        return False

    return True


def main() -> int:
    parser = argparse.ArgumentParser(description="Compile and run C++ unit tests.")
    parser.add_argument(
        "targets",
        nargs="*",
        help="Test .cpp files or directories (default: tests/).",
    )
    parser.add_argument(
        "--sanitize",
        action="store_true",
        help="Enable AddressSanitizer, LeakSanitizer, and UndefinedBehaviorSanitizer.",
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=None,
        help=(
            f"Timeout per test in seconds (default: {DEFAULT_TIMEOUT_SEC:g}, "
            f"or {DEFAULT_SANITIZE_TIMEOUT_SEC:g} with --sanitize)."
        ),
    )
    args = parser.parse_args()

    if args.timeout is not None and args.timeout <= 0:
        print("Error: --timeout must be > 0.", file=sys.stderr)
        return 1

    timeout_sec = (
        args.timeout
        if args.timeout is not None
        else (DEFAULT_SANITIZE_TIMEOUT_SEC if args.sanitize else DEFAULT_TIMEOUT_SEC)
    )

    try:
        cpp_files = discover_test_files(args.targets)
    except (FileNotFoundError, ValueError) as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1

    if not cpp_files:
        print("No .cpp files found.")
        return 0

    mode = "with sanitizers" if args.sanitize else "standard mode"
    print(f"Found {len(cpp_files)} .cpp files. Starting execution ({mode})...")

    success_count = 0
    with tempfile.TemporaryDirectory() as tmp_dir:
        bin_dir = Path(tmp_dir)
        with ThreadPoolExecutor(max_workers=MAX_WORKERS) as executor:
            futures = {
                executor.submit(
                    compile_and_run, idx, f, bin_dir, args.sanitize, timeout_sec
                ): f
                for idx, f in enumerate(cpp_files)
            }

            for future in as_completed(futures):
                if abort_event.is_set():
                    break

                if future.result():
                    success_count += 1

    if abort_event.is_set():
        print("\n[FATAL] Error occurred. Execution aborted.")
        return 1

    print(f"\n[SUCCESS] All {success_count} files compiled and executed successfully.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
