#!/usr/bin/env python3
import sys
import subprocess
import threading
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed

REPO_ROOT = Path(__file__).resolve().parent.parent
TESTS_DIR = REPO_ROOT / "tests"
MAX_WORKERS = 16
TIMEOUT_SEC = 2.0

COMPILE_CMD = [
    "g++",
    "-std=c++20",
    "-march=native",
    "-O2",
    "-Wall",
    "-Wextra",
    "-Wconversion",
    "-Wshadow",
    "-Werror",
    "-I",
    str(REPO_ROOT),
]

abort_event = threading.Event()
print_lock = threading.Lock()

def write_error_log(file_path, stage, message, stdout, stderr):
    with print_lock:
        print(f"File: {file_path}")
        print(f"Stage: {stage}")
        print(f"Error: {message}")
        print("-" * 40)
        print(f"[STDOUT]\n{stdout}")
        print("-" * 40)
        print(f"[STDERR]\n{stderr}")
        print("-" * 40 + "\n")

def compile_and_run(cpp_file):
    if abort_event.is_set():
        return False

    out_exe = cpp_file.with_suffix('.out')

    compile_args = COMPILE_CMD + [str(cpp_file), '-o', str(out_exe)]
    try:
        comp_res = subprocess.run(compile_args, capture_output=True, text=True)
        if comp_res.returncode != 0:
            abort_event.set()
            write_error_log(cpp_file, "Compilation", f"Exit code {comp_res.returncode}", comp_res.stdout, comp_res.stderr)
            return False
    except Exception as e:
        abort_event.set()
        write_error_log(cpp_file, "Compilation", str(e), "", "")
        return False

    if abort_event.is_set():
        if out_exe.exists():
            out_exe.unlink()
        return False

    try:
        run_res = subprocess.run([str(out_exe)], capture_output=True, text=True, timeout=TIMEOUT_SEC)
        if run_res.returncode != 0:
            abort_event.set()
            write_error_log(cpp_file, "Execution", f"Runtime Error (Exit code {run_res.returncode})", run_res.stdout, run_res.stderr)
            out_exe.unlink()
            return False
    except subprocess.TimeoutExpired as e:
        abort_event.set()
        write_error_log(cpp_file, "Execution", f"Timeout ({TIMEOUT_SEC}s exceeded)", e.stdout.decode() if e.stdout else "", e.stderr.decode() if e.stderr else "")
        out_exe.unlink()
        return False
    except Exception as e:
        abort_event.set()
        write_error_log(cpp_file, "Execution", str(e), "", "")
        out_exe.unlink()
        return False

    out_exe.unlink()
    return True

def main():
    if not TESTS_DIR.is_dir():
        print(f"Error: Directory '{TESTS_DIR}' not found.")
        sys.exit(1)

    cpp_files = list(TESTS_DIR.rglob('*.cpp'))
    if not cpp_files:
        print("No .cpp files found.")
        sys.exit(0)

    print(f"Found {len(cpp_files)} .cpp files. Starting execution...")

    success_count = 0
    with ThreadPoolExecutor(max_workers=MAX_WORKERS) as executor:
        futures = {executor.submit(compile_and_run, f): f for f in cpp_files}
        
        for future in as_completed(futures):
            if abort_event.is_set():
                break
            
            if future.result():
                success_count += 1

    if abort_event.is_set():
        print("\n[FATAL] Error occurred. Execution aborted.")
        sys.exit(1)
    else:
        print(f"\n[SUCCESS] All {success_count} files compiled and executed successfully.")
        sys.exit(0)

if __name__ == '__main__':
    main()
