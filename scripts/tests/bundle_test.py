#!/usr/bin/env python3
"""Unit tests for bundle.py."""

from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

REPO_ROOT = Path(__file__).resolve().parent.parent.parent
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))

import bundle  # noqa: E402


class BundleTestCase(unittest.TestCase):
    def setUp(self) -> None:
        self._tmp = tempfile.TemporaryDirectory()
        self.root = Path(self._tmp.name)

    def tearDown(self) -> None:
        self._tmp.cleanup()

    def write_files(self, files: dict[str, str]) -> None:
        for rel_path, content in files.items():
            path = self.root / rel_path
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(content, encoding="utf-8")


class BundleIncludeResolutionTest(BundleTestCase):
    def test_empty_and_no_include_files(self) -> None:
        self.write_files(
            {
                "empty.cpp": "",
                "only_newline.cpp": "\n",
                "no_trailing_nl.cpp": "int main() { return 0; }",
            }
        )
        self.assertEqual(bundle.bundle(self.root / "empty.cpp"), "")
        self.assertEqual(bundle.bundle(self.root / "only_newline.cpp"), "\n")
        self.assertEqual(
            bundle.bundle(self.root / "no_trailing_nl.cpp"),
            "int main() { return 0; }\n",
        )

    def test_missing_or_nonfile_source_raises(self) -> None:
        with self.assertRaises(FileNotFoundError):
            bundle.bundle(self.root / "missing.cpp")
        with self.assertRaises(FileNotFoundError):
            bundle.bundle(self.root)

    def test_basic_recursive_include_and_system_include_preserved(self) -> None:
        self.write_files(
            {
                "main.cpp": (
                    "#include <iostream>\n"
                    '#include "sub/a.hpp"\n'
                    "int main() { return a() + b(); }\n"
                ),
                "sub/a.hpp": '#include "b.hpp"\nint a() { return 1; }\n',
                "sub/b.hpp": "int b() { return 2; }\n",
            }
        )
        expected = (
            "#include <iostream>\n"
            "int b() { return 2; }\n"
            "int a() { return 1; }\n"
            "int main() { return a() + b(); }\n"
        )
        self.assertEqual(bundle.bundle(self.root / "main.cpp"), expected)

    def test_include_search_priority(self) -> None:
        self.write_files(
            {
                "src/main.cpp": (
                    '#include "local.hpp"\n'
                    '#include "order.hpp"\n'
                ),
                "src/local.hpp": "int local_src();\n",
                "inc1/local.hpp": "int local_inc1();\n",
                "inc1/order.hpp": "int order_inc1();\n",
                "inc2/order.hpp": "int order_inc2();\n",
            }
        )
        result = bundle.bundle(
            self.root / "src/main.cpp",
            include_dirs=[self.root / "inc1", self.root / "inc2"],
        )
        self.assertEqual(result, "int local_src();\nint order_inc1();\n")

    def test_repo_root_fallback_include(self) -> None:
        self.write_files(
            {
                "main.cpp": '#include "library3372/math/isqrt.hpp"\n',
            }
        )
        result = bundle.bundle(self.root / "main.cpp")
        self.assertIn("namespace library3372", result)
        self.assertIn("isqrt", result)
        self.assertNotIn('#include "library3372/math/isqrt.hpp"', result)

    def test_missing_included_file_raises(self) -> None:
        self.write_files({"main.cpp": '#include "nope.hpp"\n'})
        with self.assertRaises(FileNotFoundError) as ctx:
            bundle.bundle(self.root / "main.cpp")
        self.assertIn("nope.hpp", str(ctx.exception))

    def test_utf8_content_preserved(self) -> None:
        self.write_files(
            {
                "main.cpp": '#include "u.hpp"\n// \u30c6\u30b9\u30c8\n',
                "u.hpp": 'const char* s = "\u3042\u3044";\n',
            }
        )
        self.assertEqual(
            bundle.bundle(self.root / "main.cpp"),
            'const char* s = "\u3042\u3044";\n// \u30c6\u30b9\u30c8\n',
        )


class BundlePragmaAndDeduplicationTest(BundleTestCase):
    def test_diamond_circular_and_self_include_deduplicated(self) -> None:
        self.write_files(
            {
                "main.cpp": (
                    '#include "main.cpp"\n'
                    '#include "a.hpp"\n'
                    '#include "b.hpp"\n'
                    "int main() {}\n"
                ),
                "a.hpp": '#include "common.hpp"\n#include "b.hpp"\nint a();\n',
                "b.hpp": '#include "./sub/../common.hpp"\n#include "a.hpp"\nint b();\n',
                "common.hpp": "int common();\n",
            }
        )
        (self.root / "sub").mkdir()
        result = bundle.bundle(self.root / "main.cpp")
        self.assertEqual(
            result,
            "int common();\nint b();\nint a();\nint main() {}\n",
        )

    def test_pragma_once_stripped_only_in_included_files(self) -> None:
        self.write_files(
            {
                "main.cpp": (
                    "#pragma once\n"
                    '#include "a.hpp"\n'
                    '#include "b.hpp"\n'
                    "int m;\n"
                ),
                "a.hpp": "#  pragma   once  // comment\nint a;\n",
                "b.hpp": "#pragma once extra_token\nint b;\n",
            }
        )
        result = bundle.bundle(self.root / "main.cpp")
        self.assertEqual(
            result,
            "#pragma once\nint a;\n#pragma once extra_token\nint b;\nint m;\n",
        )

    def test_other_pragmas_preserved(self) -> None:
        self.write_files(
            {
                "main.cpp": '#include "a.hpp"\n',
                "a.hpp": '#pragma once\n#pragma GCC optimize("O3")\nint x;\n',
            }
        )
        self.assertEqual(
            bundle.bundle(self.root / "main.cpp"),
            '#pragma GCC optimize("O3")\nint x;\n',
        )


class BundleLexerAndCommentsTest(BundleTestCase):
    def test_directive_with_surrounding_comments_and_trailing_tokens(self) -> None:
        self.write_files(
            {
                "main.cpp": (
                    '  #  include  "a.hpp"  // trailing line comment\n'
                    '/* c1 */ /* c2 */ #include "b.hpp" /* trailing */\n'
                    '/* unclosed #include "missing.hpp"\n'
                    '*/ #include "c.hpp"\n'
                    '#include "d.hpp" int not_a_directive;\n'
                    '#include "e.hpp" /* start_comment\n'
                    '#include "missing.hpp"\n'
                    "*/\n"
                ),
                "a.hpp": "int a;\n",
                "b.hpp": "int b;\n",
                "c.hpp": "int c;\n",
                "d.hpp": "int d;\n",
                "e.hpp": "int e;\n",
            }
        )
        expected = (
            "int a;\n"
            "int b;\n"
            '/* unclosed #include "missing.hpp"\n'
            "*/\n"
            "int c;\n"
            '#include "d.hpp" int not_a_directive;\n'
            "int e;\n"
            "/* start_comment\n"
            '#include "missing.hpp"\n'
            "*/\n"
        )
        self.assertEqual(bundle.bundle(self.root / "main.cpp"), expected)

    def test_pragma_once_with_multiline_comments(self) -> None:
        self.write_files(
            {
                "main.cpp": '#include "a.hpp"\n',
                "a.hpp": (
                    "/* header\n"
                    "*/ #pragma once /* trailing\n"
                    '#include "missing.hpp"\n'
                    "*/\n"
                    "int ok;\n"
                ),
            }
        )
        expected = (
            "/* header\n"
            "*/\n"
            "/* trailing\n"
            '#include "missing.hpp"\n'
            "*/\n"
            "int ok;\n"
        )
        self.assertEqual(bundle.bundle(self.root / "main.cpp"), expected)

    def test_line_comment_backslash_continuation(self) -> None:
        self.write_files(
            {
                "main.cpp": (
                    "// continued comment \\\n"
                    '#include "missing.hpp"\n'
                    "// not continued \\\\\n"
                    '#include "ok.hpp"\n'
                ),
                "ok.hpp": "int ok;\n",
            }
        )
        expected = (
            "// continued comment \\\n"
            '#include "missing.hpp"\n'
            "// not continued \\\\\n"
            "int ok;\n"
        )
        self.assertEqual(bundle.bundle(self.root / "main.cpp"), expected)

    def test_multiline_string_literal_and_escapes(self) -> None:
        self.write_files(
            {
                "main.cpp": (
                    'const char* s1 = "escaped \\" quote \\\n'
                    '#include "missing.hpp"\n'
                    '\\\\\\\n'
                    '#pragma once\n'
                    '";\n'
                    '#include "ok.hpp"\n'
                ),
                "ok.hpp": "int ok;\n",
            }
        )
        expected = (
            'const char* s1 = "escaped \\" quote \\\n'
            '#include "missing.hpp"\n'
            '\\\\\\\n'
            '#pragma once\n'
            '";\n'
            "int ok;\n"
        )
        self.assertEqual(bundle.bundle(self.root / "main.cpp"), expected)

    def test_raw_string_literals_and_identifier_prefix(self) -> None:
        self.write_files(
            {
                "main.cpp": (
                    'const char* r1 = R"(\n'
                    '#include "missing.hpp"\n'
                    ')";\n'
                    'const char* r2 = u8R"DELIM(\n'
                    ')" still inside raw\n'
                    '#include "missing.hpp"\n'
                    ')DELIM";\n'
                    'const char* r3 = LR"(\n'
                    '#include "missing.hpp"\n'
                    ')";\n'
                    'const char* not_raw = fooR"(");\n'
                    '#include "ok.hpp"\n'
                ),
                "ok.hpp": "int ok;\n",
            }
        )
        result = bundle.bundle(self.root / "main.cpp")
        self.assertIn("int ok;\n", result)
        self.assertNotIn('#include "ok.hpp"', result)

    def test_char_literals_and_digit_separators(self) -> None:
        self.write_files(
            {
                "main.cpp": (
                    "char c1 = '\"';\n"
                    "char c2 = '\\'';\n"
                    "char c3 = '\\\\';\n"
                    "int n1 = 1'000'000; double d1 = 1.5e+2; /*\n"
                    '#include "missing.hpp"\n'
                    "*/\n"
                    "auto h1 = 0x1p-2'0; /*\n"
                    '#include "missing.hpp"\n'
                    "*/\n"
                    '#include "ok.hpp"\n'
                ),
                "ok.hpp": "int ok;\n",
            }
        )
        result = bundle.bundle(self.root / "main.cpp")
        self.assertIn("int ok;\n", result)
        self.assertNotIn('#include "ok.hpp"', result)


class BundleCliTest(BundleTestCase):
    def run_cli(self, *args: str) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            [sys.executable, str(REPO_ROOT / "bundle.py"), *args],
            capture_output=True,
            text=True,
        )

    def test_cli_stdout_and_output_file_and_include_dir(self) -> None:
        self.write_files(
            {
                "main.cpp": '#include "inc.hpp"\nint main() { return x; }\n',
                "headers/inc.hpp": "int x = 42;\n",
            }
        )
        expected = "int x = 42;\nint main() { return x; }\n"

        res_stdout = self.run_cli(
            str(self.root / "main.cpp"), "-I", str(self.root / "headers")
        )
        self.assertEqual(res_stdout.returncode, 0)
        self.assertEqual(res_stdout.stdout, expected)
        self.assertEqual(res_stdout.stderr, "")

        out_path = self.root / "bundled.cpp"
        res_file = self.run_cli(
            str(self.root / "main.cpp"),
            "--include-dir",
            str(self.root / "headers"),
            "-o",
            str(out_path),
        )
        self.assertEqual(res_file.returncode, 0)
        self.assertEqual(res_file.stdout, "")
        self.assertEqual(out_path.read_text(encoding="utf-8"), expected)

    def test_cli_errors_return_nonzero(self) -> None:
        res_missing_src = self.run_cli(str(self.root / "missing.cpp"))
        self.assertEqual(res_missing_src.returncode, 1)
        self.assertIn("Error:", res_missing_src.stderr)

        self.write_files({"main.cpp": "int x;\n"})
        res_bad_out = self.run_cli(
            str(self.root / "main.cpp"), "-o", str(self.root / "no_dir/out.cpp")
        )
        self.assertEqual(res_bad_out.returncode, 1)
        self.assertIn("Error:", res_bad_out.stderr)


if __name__ == "__main__":
    unittest.main()
