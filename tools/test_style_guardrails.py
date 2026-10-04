#!/usr/bin/env python3
"""Exercise the gd naming policy with real clang-tidy and clang-query diagnostics."""

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
import hungarian_prefixes


class NamingGuardrailTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tidy_path = find_tool("clang-tidy")
        cls.query_path = find_tool("clang-query")
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

    def prefix_violations(self, source_text):
        with tempfile.TemporaryDirectory(prefix="aoc-prefix-") as temporary_directory:
            fixture_root = Path(temporary_directory).resolve()
            source_path = fixture_root / "src" / "fixture.cpp"
            source_path.parent.mkdir()
            source_path.write_text(source_text)
            # Standalone clang-query finds libc++ through the SDK root; an explicit
            # -isystem would precede clang's own headers and break <cmath>.
            sysroot_arguments = self.compiler_arguments[:2]
            violations = hungarian_prefixes.check_prefixes(
                self.query_path, [source_path], [], fixture_root, ["-std=c++20", *sysroot_arguments]
            )
            return {declaration.name for declaration, _ in violations}

    def test_gd_names_pass_capitalization(self):
        result = self.check_fixture('''
            struct JunctionBox { long m_iX; long m_iY; long m_iZ; };
            class Solver { int m_iPressCount = 0; };
            constexpr int iButtonCount = 4;
            template <typename FUNCTION> int FewestPresses(int iLightCount, FUNCTION functionNext) {
                JunctionBox junctionbox{1, 2, 3};
                auto [iX, iY, iZ] = junctionbox;
                auto next_button_ = [](auto uButtonIndex) { return uButtonIndex + 1; };
                int arrayPivotRowByColumn[4]{};
                bool bReachable = true;
                for (int i = 0; i < 1; ++i) {}
                return iX + iY + iZ + next_button_(iLightCount) + iButtonCount + functionNext()
                       + arrayPivotRowByColumn[0] + bReachable;
            }
        ''')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_invalid_capitalization_fails_in_each_declaration_category(self):
        fixtures = {
            "snake local": "int Solve() { int row_count = 1; return row_count; }",
            "snake parameter": "int Solve(int button_count) { return button_count; }",
            "field without m_": "struct Cell { int iFirstRow; };",
            "private field suffix": "class Solver { int iPressCount_; };",
            "constant": "constexpr int MAX_ROWS = 4;",
            "structured binding": "struct Cell { int m_iRow; int m_iColumn; }; void Solve() { auto [FirstRow, FirstColumn] = Cell{1, 2}; }",
            "lambda parameter": "auto advance_ = [](int ButtonIndex) { return ButtonIndex; };",
            "template parameter": "template <typename Function> void Solve() {}",
            "procedure": "int parse_input() { return 0; }",
            "method": "struct Cell { int get_row() { return 0; } };",
            "type": "struct cell {};",
        }
        for category, source_text in fixtures.items():
            with self.subTest(category=category):
                result = self.check_fixture(source_text)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("readability-identifier-naming", result.stdout + result.stderr)
                self.assertNotIn("clang-diagnostic-error", result.stdout + result.stderr)

    def test_prefixes_match_declared_types(self):
        violations = self.prefix_violations('''
            #include <cstdint>
            #include <map>
            #include <memory>
            #include <optional>
            #include <string>
            #include <string_view>
            #include <utility>
            #include <vector>
            struct JunctionBox { std::int64_t m_iX; std::size_t m_uIndex; };
            struct TreeRegion { int m_iWidth; std::vector<int> m_vectorPresentCounts; };
            int Solve(const std::vector<int>& vectorValues, std::string_view stringLine, const char* pbszText,
                      double dRate, bool bReady, TreeRegion treeregionFirst) {
                std::size_t uCount = vectorValues.size();
                auto itFirst = vectorValues.begin();
                std::map<int, int> mapCountsByRow;
                std::optional<int> optionalBest;
                auto pjunctionbox = std::make_unique<JunctionBox>();
                auto treeregion = treeregionFirst;
                char iSymbol = stringLine.front();
                auto count_rows_ = [&](int iRow) { return iRow + static_cast<int>(uCount); };
                for (const auto& iValue : vectorValues) { (void)iValue; }
                for (std::size_t u = 0; u < uCount; ++u) {}
                auto [iFirst, iSecond] = std::pair<int, int>{1, 2};
                return count_rows_(*itFirst) + mapCountsByRow.size() + optionalBest.value_or(0) + pjunctionbox->m_iX
                       + treeregion.m_iWidth + iSymbol + iFirst + iSecond + (pbszText != nullptr) + bReady
                       + static_cast<int>(dRate);
            }
        ''')
        self.assertEqual(violations, set())

    def test_prefixes_that_contradict_types_fail(self):
        violations = self.prefix_violations('''
            #include <cstdint>
            #include <string>
            #include <utility>
            #include <vector>
            struct JunctionBox { std::int64_t m_uX; int iMissingMember; };
            struct TreeRegion { int m_iWidth; };
            int Solve(const std::vector<int>& listValues, std::string vectorName, unsigned iCount, TreeRegion regionFirst) {
                std::size_t iSize = listValues.size();
                bool iReady = true;
                double uRate = 1.0;
                const char* stringText = "x";
                auto counter = [](int iRow) { return iRow; };
                for (const auto& uValue : listValues) { (void)uValue; }
                auto [uFirst, bSecond] = std::pair<int, int>{1, 2};
                return static_cast<int>(iSize) + iReady + static_cast<int>(uRate) + (stringText != nullptr)
                       + counter(1) + static_cast<int>(vectorName.size()) + static_cast<int>(iCount)
                       + regionFirst.m_iWidth + uFirst + bSecond;
            }
        ''')
        self.assertEqual(violations, {
            "m_uX", "iMissingMember", "listValues", "vectorName", "iCount", "regionFirst", "iSize", "iReady",
            "uRate", "stringText", "counter", "uValue", "uFirst", "bSecond",
        })

    def test_main_parameters_have_gd_names(self):
        check_main_names("int main(int iArgumentCount, char** ppbszArgument) {}")
        with self.assertRaisesRegex(RuntimeError, "entry point"):
            check_main_names("int main(int argc, char** argv) {}")

    def test_comments_and_literals_cannot_bypass_main_check(self):
        for decoy in (
            "// int main(int iArgumentCount, char** ppbszArgument) {}\n",
            "/* int main(int iArgumentCount, char** ppbszArgument) {} */\n",
            'const char* pbszText = "int main(int iArgumentCount, char** ppbszArgument) {}";\n',
            'const char* pbszText = R"tag(int main(int iArgumentCount, char** ppbszArgument) {})tag";\n',
        ):
            with self.subTest(decoy=decoy):
                with self.assertRaisesRegex(RuntimeError, "entry point"):
                    check_main_names(decoy + "int main(int argc, char** argv) {}")

    def test_compilation_errors_do_not_pass(self):
        result = self.check_fixture("int Solve() { return missing; }")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("clang-diagnostic-error", result.stdout + result.stderr)
        with self.assertRaisesRegex(RuntimeError, "compiler errors|clang-query failed"):
            self.prefix_violations("int Solve() { return missing; }")

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
