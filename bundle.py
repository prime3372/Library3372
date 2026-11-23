#!/usr/bin/env python3
"""Bundle a C++ source file by recursively expanding local #include directives."""

import argparse
from dataclasses import dataclass
from pathlib import Path
import re
import sys

REPO_ROOT = Path(__file__).resolve().parent
INCLUDE_RE = re.compile(r'^#\s*include\s*"([^"]+)"(.*)$')
PRAGMA_ONCE_RE = re.compile(r'^#\s*pragma\s+once\b(.*)$')
RAW_STRING_START_RE = re.compile(r'(?:u8|u|U|L)?R"([^\s()\\]{0,16})\(')


@dataclass
class LexerState:
    in_block_comment: bool = False
    raw_delim: str | None = None
    in_string_cont: bool = False
    in_line_comment_cont: bool = False

    def can_start_directive(self) -> bool:
        return (
            self.raw_delim is None
            and not self.in_string_cont
            and not self.in_line_comment_cont
        )


def has_trailing_backslash(line: str) -> bool:
    """Return True if line ends with an odd number of backslashes."""
    return (len(line) - len(line.rstrip("\\"))) % 2 == 1


def strip_leading_ws_and_comments(
    text: str, *, in_block_comment: bool = False
) -> tuple[str, str, bool]:
    """Strip leading whitespace and complete /* ... */ blocks.

    Returns (stripped_prefix, remaining_text, has_unclosed_block_comment).
    """
    idx = 0
    n = len(text)
    if in_block_comment:
        end = text.find("*/")
        if end == -1:
            return text, "", True
        idx = end + 2

    while idx < n:
        while idx < n and text[idx].isspace():
            idx += 1
        if text.startswith("/*", idx):
            end = text.find("*/", idx + 2)
            if end == -1:
                return text, "", True
            idx = end + 2
        else:
            break
    return text[:idx], text[idx:], False


def advance_lexer_state(line: str, state: LexerState) -> None:
    """Update multi-line comment and string literal state across one physical line."""
    if state.in_line_comment_cont:
        state.in_line_comment_cont = has_trailing_backslash(line)
        return

    i = 0
    n = len(line)
    in_string = state.in_string_cont
    state.in_string_cont = False

    while i < n or in_string:
        if state.raw_delim is not None:
            closing = f"){state.raw_delim}\""
            end = line.find(closing, i)
            if end == -1:
                return
            i = end + len(closing)
            state.raw_delim = None
            continue

        if state.in_block_comment:
            end = line.find("*/", i)
            if end == -1:
                return
            i = end + 2
            state.in_block_comment = False
            continue

        if in_string:
            if i >= n:
                return
            while i < n:
                ch = line[i]
                if ch == "\\":
                    if i + 1 == n:
                        state.in_string_cont = True
                        return
                    i += 2
                elif ch == '"':
                    in_string = False
                    i += 1
                    break
                else:
                    i += 1
            continue

        ch = line[i]
        if line.startswith("//", i):
            state.in_line_comment_cont = has_trailing_backslash(line)
            return

        if line.startswith("/*", i):
            state.in_block_comment = True
            i += 2
            continue

        prev_is_ident = i > 0 and (line[i - 1].isalnum() or line[i - 1] == "_")
        if not prev_is_ident:
            raw_match = RAW_STRING_START_RE.match(line, i)
            if raw_match:
                state.raw_delim = raw_match.group(1)
                i = raw_match.end()
                continue
            # Consume numeric literals so digit separators (e.g. 1'000) do not open char literals.
            if ch.isdigit():
                i += 1
                while i < n:
                    cur = line[i]
                    if (
                        cur in ("e", "E", "p", "P")
                        and i + 1 < n
                        and line[i + 1] in ("+", "-")
                    ):
                        i += 2
                    elif cur.isalnum() or cur in ("_", "'"):
                        i += 1
                    else:
                        break
                continue

        if ch == '"':
            in_string = True
            i += 1
            continue

        if ch == "'":
            i += 1
            while i < n:
                if line[i] == "\\":
                    i += 2
                elif line[i] == "'":
                    i += 1
                    break
                else:
                    i += 1
            continue

        i += 1


def resolve_include(
    target: str, current_file: Path, include_dirs: tuple[Path, ...]
) -> Path:
    """Resolve a quoted #include path via current dir, -I dirs, and repo root."""
    search_dirs = (current_file.parent, *include_dirs, REPO_ROOT)
    for base_dir in search_dirs:
        candidate = (base_dir / target).resolve()
        if candidate.is_file():
            return candidate
    raise FileNotFoundError(
        f"Included file '{target}' not found (included from '{current_file}')."
    )


def match_directive(
    line: str, state: LexerState
) -> tuple[str, str, str, str, bool] | None:
    """Match #include "..." or #pragma once outside comments and string literals.

    Returns (kind, arg, leading_prefix, trailing_rest, trailing_unclosed) or None.
    """
    if not state.can_start_directive():
        return None

    prefix, stripped, unclosed = strip_leading_ws_and_comments(
        line, in_block_comment=state.in_block_comment
    )
    if unclosed or not stripped.startswith("#"):
        return None

    inc_match = INCLUDE_RE.match(stripped)
    if inc_match:
        trailing = inc_match.group(2)
        _, rest, rest_unclosed = strip_leading_ws_and_comments(trailing)
        if rest == "" or rest.startswith("//"):
            return ("include", inc_match.group(1), prefix, trailing, rest_unclosed)
        return None

    pragma_match = PRAGMA_ONCE_RE.match(stripped)
    if pragma_match:
        trailing = pragma_match.group(1)
        _, rest, rest_unclosed = strip_leading_ws_and_comments(trailing)
        if rest == "" or rest.startswith("//"):
            return ("pragma_once", "", prefix, trailing, rest_unclosed)

    return None


def expand_file(
    file_path: Path,
    visited: set[Path],
    include_dirs: tuple[Path, ...],
    *,
    is_included: bool = False,
) -> list[str]:
    """Recursively expand quoted #include directives while preserving comments."""
    lines = file_path.read_text(encoding="utf-8").splitlines()
    expanded: list[str] = []
    state = LexerState()

    for line in lines:
        directive = match_directive(line, state)
        if directive is not None:
            kind, arg, prefix, trailing, rest_unclosed = directive
            if state.in_block_comment and prefix.strip():
                expanded.append(prefix.rstrip())
            state.in_block_comment = rest_unclosed

            if kind == "pragma_once" and is_included:
                if rest_unclosed and trailing.strip():
                    expanded.append(trailing.lstrip())
                continue

            if kind == "include":
                included_path = resolve_include(arg, file_path, include_dirs)
                if included_path not in visited:
                    visited.add(included_path)
                    expanded.extend(
                        expand_file(
                            included_path,
                            visited,
                            include_dirs,
                            is_included=True,
                        )
                    )
                if rest_unclosed and trailing.strip():
                    expanded.append(trailing.lstrip())
                continue

        advance_lexer_state(line, state)
        expanded.append(line)

    return expanded


def bundle(
    source_path: Path, include_dirs: list[Path] | tuple[Path, ...] = ()
) -> str:
    """Bundle source_path into a single C++ source string."""
    resolved_source = source_path.resolve()
    if not resolved_source.is_file():
        raise FileNotFoundError(f"Source file '{source_path}' not found.")

    resolved_include_dirs = tuple(d.resolve() for d in include_dirs)
    visited: set[Path] = {resolved_source}
    lines = expand_file(resolved_source, visited, resolved_include_dirs)
    return ("\n".join(lines) + "\n") if lines else ""


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Expand quoted #include directives in a C++ source file."
    )
    parser.add_argument("source", help="Path to the entry C++ source file.")
    parser.add_argument(
        "-I",
        "--include-dir",
        dest="include_dirs",
        action="append",
        default=[],
        help="Additional include search directory.",
    )
    parser.add_argument(
        "-o",
        "--output",
        help="Path to write the bundled output (defaults to stdout).",
    )
    args = parser.parse_args()

    try:
        bundled = bundle(
            Path(args.source), [Path(d) for d in args.include_dirs]
        )
        if args.output:
            Path(args.output).write_text(bundled, encoding="utf-8")
        else:
            sys.stdout.write(bundled)
    except (FileNotFoundError, OSError) as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
