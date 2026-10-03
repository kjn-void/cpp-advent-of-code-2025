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

from check_style import CheckMainNames, PathFindTool, pathRoot, RgargPlatform, RgpathTranslationUnits, RunChecked


class NamingGuardrailTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.pathTidy = PathFindTool("clang-tidy")
        cls.rgargCompiler = RgargPlatform()

    def RecCheckFixture(self, usSource):
        with tempfile.TemporaryDirectory(prefix="aoc-naming-") as pathTemporary:
            pathSource = Path(pathTemporary) / "fixture.cpp"
            pathSource.write_text(usSource)
            return subprocess.run(
                [self.pathTidy, str(pathSource), f"--config-file={pathRoot / '.clang-tidy'}",
                 "--quiet", "--", "-std=c++20", *self.rgargCompiler],
                text=True, capture_output=True, check=False,
            )

    def test_semantic_tags_and_compositions(self):
        recCheck = self.RecCheckFixture('''
            struct Cel { int rw; long col; };
            constexpr int crw = 4;
            int ValSolve(const char* pchLim, int idDay) {
                Cel cel{1, 2};
                auto [rwFirst, colFirst] = cel;
                double costMinimum = 0;
                auto fnAdvance = [](auto ibtn) { return ibtn + 1; };
                int valParsed = 3;
                int mpcolrwPivot[4]{};
                bool fReady = true;
                return rwFirst + colFirst + valParsed + fnAdvance(idDay) + crw
                       + mpcolrwPivot[0] + fReady
                       + static_cast<int>(costMinimum) + (pchLim != nullptr);
            }
        ''')
        self.assertEqual(recCheck.returncode, 0, recCheck.stdout + recCheck.stderr)

    def test_invalid_names_fail_in_each_declaration_category(self):
        mptxtusFixture = {
            "local": "int ValSolve() { int rows = 1; return rows; }",
            "parameter": "int ValSolve(int count) { return count; }",
            "field": "struct Cel { int row; };",
            "constant": "constexpr int MAX_ROWS = 4;",
            "structured binding": "struct Cel { int rw; int col; }; void Solve() { auto [row, column] = Cel{1, 2}; }",
            "lambda parameter": "auto fnTest = [](int n) { return n; };",
            "loop": "void Solve() { for (int i = 0; i < 3; ++i) {} }",
            "storage prefix": "int ValSolve() { int iCount = 1; return iCount; }",
            "pointer without domain": "void Solve(const char* pName) {}",
            "former custom dialect": "void Solve(int idxButton) {}",
            "nonstandard composition": "void Solve(int cRows) {}",
            "procedure": "int parse_input() { return 0; }",
            "method": "struct Cel { int get_row() { return 0; } };",
            "type": "struct cell {};",
        }
        for txtCategory, usSource in mptxtusFixture.items():
            with self.subTest(category=txtCategory):
                recCheck = self.RecCheckFixture(usSource)
                self.assertNotEqual(recCheck.returncode, 0)
                self.assertIn("readability-identifier-naming", recCheck.stdout + recCheck.stderr)
                self.assertNotIn("clang-diagnostic-error", recCheck.stdout + recCheck.stderr)

    def test_main_parameters_have_semantic_names(self):
        CheckMainNames("int main(int cusArgs, char** rgusArgs) {}")
        with self.assertRaisesRegex(RuntimeError, "entry point"):
            CheckMainNames("int main(int argc, char** argv) {}")

    def test_comments_and_literals_cannot_bypass_main_check(self):
        for usDecoy in (
            "// int main(int cusArgs, char** rgusArgs) {}\n",
            "/* int main(int cusArgs, char** rgusArgs) {} */\n",
            'const char* us = "int main(int cusArgs, char** rgusArgs) {}";\n',
            'const char* us = R"tag(int main(int cusArgs, char** rgusArgs) {})tag";\n',
        ):
            with self.subTest(decoy=usDecoy):
                with self.assertRaisesRegex(RuntimeError, "entry point"):
                    CheckMainNames(usDecoy + "int main(int argc, char** argv) {}")

    def test_compilation_errors_do_not_pass(self):
        recCheck = self.RecCheckFixture("int ValSolve() { return missing; }")
        self.assertNotEqual(recCheck.returncode, 0)
        self.assertIn("clang-diagnostic-error", recCheck.stdout + recCheck.stderr)

    def test_ci_diagnostics_preserve_failure_and_escape_annotations(self):
        outCapture = io.StringIO()
        with mock.patch.dict(os.environ, {"GITHUB_ACTIONS": "true"}):
            with contextlib.redirect_stdout(outCapture):
                with self.assertRaises(subprocess.CalledProcessError) as errProcess:
                    RunChecked([sys.executable, "-c", "import sys; print('bad%\\nname'); sys.exit(7)"])
        self.assertEqual(errProcess.exception.returncode, 7)
        self.assertIn("::error title=Style validation::bad%25%0Aname%0A", outCapture.getvalue())

    def test_missing_translation_units_do_not_pass(self):
        with tempfile.TemporaryDirectory(prefix="aoc-database-") as pathTemporary:
            pathBuild = Path(pathTemporary)
            (pathBuild / "compile_commands.json").write_text("[]")
            with self.assertRaisesRegex(RuntimeError, "omits first-party sources"):
                RgpathTranslationUnits(pathBuild, [pathRoot / "src/main.cpp"])


if __name__ == "__main__":
    unittest.main()
