#!/usr/bin/env python3
"""Exercise the naming policy with real clang-tidy diagnostics."""

import contextlib
import io
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

from check_style import check_main_names, find_tool, repo_root, platform_arguments, translation_units, run_checked


class NamingGuardrailTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tidy_path = find_tool("clang-tidy")
        cls.compiler_arguments = platform_arguments()

    def check_fixture(self, source_text):
        with tempfile.TemporaryDirectory(prefix="aoc-naming-") as temporary_directory:
            source_path = Path(temporary_directory) / "fixture.cpp"
            source_path.write_text(source_text)
            return subprocess.run(
                [self.tidy_path, str(source_path), f"--config-file={repo_root / '.clang-tidy'}",
                 "--quiet", "--", "-std=c++20", *self.compiler_arguments],
                text=True, capture_output=True, check=False,
            )

    def test_domain_names_and_standard_coordinates(self):
        result = self.check_fixture(''' 
            struct JunctionBox { long x; long y; long z; };
            class Solver { int press_count_ = 0; };
            constexpr int button_count = 4;
            int fewest_presses(int light_count) {
                JunctionBox box{1, 2, 3};
                auto [x, y, z] = box;
                auto next_button = [](auto button_index) { return button_index + 1; };
                int pivot_row_by_column[4]{};
                bool reachable = true;
                return x + y + z + next_button(light_count) + button_count
                       + pivot_row_by_column[0] + reachable;
            }
        ''')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_invalid_names_fail_in_each_declaration_category(self):
        fixtures = {
            "local": "int solve() { int rowCount = 1; return rowCount; }",
            "parameter": "int solve(int buttonCount) { return buttonCount; }",
            "field": "struct Cell { int firstRow; };",
            "private field": "class Solver { int pressCount_; };",
            "constant": "constexpr int MAX_ROWS = 4;",
            "structured binding": "struct Cell { int row; int column; }; void solve() { auto [firstRow, firstColumn] = Cell{1, 2}; }",
            "lambda parameter": "auto advance = [](int buttonIndex) { return buttonIndex; };",
            "loop": "void solve() { for (int rowIndex = 0; rowIndex < 3; ++rowIndex) {} }",
            "hungarian dialect": "void solve(int cButtons) {}",
            "procedure": "int ParseInput() { return 0; }",
            "method": "struct Cell { int GetRow() { return 0; } };",
            "type": "struct cell {};",
        }
        for category, source_text in fixtures.items():
            with self.subTest(category=category):
                result = self.check_fixture(source_text)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("readability-identifier-naming", result.stdout + result.stderr)
                self.assertNotIn("clang-diagnostic-error", result.stdout + result.stderr)

    def test_main_parameters_have_semantic_names(self):
        check_main_names("int main(int argc, char** argv) {}")
        with self.assertRaisesRegex(RuntimeError, "entry point"):
            check_main_names("int main(int count, char** arguments) {}")

    def test_comments_and_literals_cannot_bypass_main_check(self):
        for decoy in (
            "// int main(int argc, char** argv) {}\n",
            "/* int main(int argc, char** argv) {} */\n",
            'const char* us = "int main(int argc, char** argv) {}";\n',
            'const char* us = R"tag(int main(int argc, char** argv) {})tag";\n',
        ):
            with self.subTest(decoy=decoy):
                with self.assertRaisesRegex(RuntimeError, "entry point"):
                    check_main_names(decoy + "int main(int count, char** arguments) {}")

    def test_compilation_errors_do_not_pass(self):
        result = self.check_fixture("int solve() { return missing; }")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("clang-diagnostic-error", result.stdout + result.stderr)

    def test_ci_diagnostics_preserve_failure_and_escape_annotations(self):
        captured_output = io.StringIO()
        with mock.patch.dict(os.environ, {"GITHUB_ACTIONS": "true"}):
            with contextlib.redirect_stdout(captured_output):
                with self.assertRaises(subprocess.CalledProcessError) as process_error:
                    run_checked([sys.executable, "-c", "import sys; print('bad%\\nname'); sys.exit(7)"])
        self.assertEqual(process_error.exception.returncode, 7)
        self.assertIn("::error title=Style validation::bad%25%0Aname%0A", captured_output.getvalue())

    def test_missing_translation_units_do_not_pass(self):
        with tempfile.TemporaryDirectory(prefix="aoc-database-") as temporary_directory:
            build_directory = Path(temporary_directory)
            (build_directory / "compile_commands.json").write_text("[]")
            with self.assertRaisesRegex(RuntimeError, "omits first-party sources"):
                translation_units(build_directory, [repo_root / "src/main.cpp"])


if __name__ == "__main__":
    unittest.main()
