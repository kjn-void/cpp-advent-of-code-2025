# Project conventions

Use idiomatic C++20 and the Systems Hungarian style of the `gd` library on the
`hungarian` branch: a short type prefix followed by the full domain words. Refer to
the original [Advent of Code 2025 problems](https://adventofcode.com/2025) and the
puzzle vocabulary in [README.md](README.md). The `main` branch uses ordinary
snake_case names; keep the two branches' algorithms identical.

## Variable names

Every variable, parameter, constant, member, and structured binding starts with the
prefix for its C++ type, then the domain words in CamelCase. Never abbreviate the
domain words: the name must stay greppable (`iButtonCount`, not `iBtnCnt`).

| Prefix | Type | Example |
| --- | --- | --- |
| `b` | `bool` | `bCanDiscard` |
| `i` | signed integers, including `std::int64_t` and `char` used as a value | `iPressCount`, `iDigit` |
| `u` | unsigned integers, including `std::size_t` | `uButtonIndex` |
| `d` | `float`, `double` | `dRate` |
| `e` | enum values | `eParseError` |
| `p` | raw or smart pointer, followed by the pointee's prefix | `psolution`, `pvectorActiveBeams` |
| `pbsz` | pointer to a zero-terminated character string | `pbszExampleText` |
| `it` | iterator | `itFound` |
| `string` | `std::string`, `std::string_view` | `stringLine` |
| `vector`, `array`, `pair`, `map`, `set`, `queue`, `optional`, `span`, `function`, `atomic` | the standard template of that name (`map` and `set` include the unordered forms) | `vectorPivotRowByColumn`, `m_mapOutputsByDevice` |
| lowercase class name | any other class, without namespace or underscores | `junctionbox`, `treeregionFirst`, `istringstreamCounts` |
| lowercase template parameter | a value whose type is a template parameter | `functionValueAt`, `integerValue` |

- Members start with `m_` before the prefix: `m_iRowCount`, `m_vectorRedTiles`. This
  applies to plain record fields too (`JunctionBox::m_iX`).
- Drop words that only repeat the class prefix: a `JunctionBox` parameter is
  `junctionbox` or `junctionboxFirst`, and a `MachineDefinition` is
  `machinedefinition`.
- Short loop counters may be the bare prefix: `i`, `u`, `it`.
- Escape hatch: a lowercase name ending in `_` turns off the prefix rule for a value
  whose declaration is verbose and whose use is local, such as a lambda
  (`count_paths_`, `recurse_`) or an unused loop variable (`iteration_`). Lambdas
  always use this form. Members never do.
- `main` is `int main(int iArgumentCount, char** ppbszArgument)`.

Name what a value means. Distinguish `iButtonCount` from `uButtonIndex`,
`iSquaredDistance` from a distance, and `vectorJoltageRequirements` from
`vectorRemainingJoltage`. Use `First`/`Last` for inclusive endpoints and `End` for an
exclusive endpoint. Keep spatial `X`, `Y`, `Z` separate from grid `Row`/`Column` and
from collection indices. Keep clear algorithm terms such as pivot, parity, prefix
sum, and union-find. Predicates read as a true condition: `bIsHorizontal`.

## Functions and types

- Functions and methods use PascalCase with no prefix and as few words as needed:
  `SetInput`, `Part1`, `FewestPressesForJoltage`, `core::ParseInteger`.
- Classes, structs, and type aliases use PascalCase; prefer the puzzle's nouns
  (`JunctionBox`, `BoxPair`, `MachineDefinition`, `PresentShape`, `TreeRegion`).
- Template type parameters are UPPER_CASE: `FUNCTION`, `INTEGER`, `DAY_SOLVER`.
- Keep `DayNN` solver names, file, target and CLI names, GoogleTest/Benchmark macro
  names, and CMake options.
- Python tools follow PEP 8 and CMake keeps its usual lowercase variables; the gd
  style applies to C++.

## Guardrails

- `.clang-tidy` checks capitalization, `m_`, and UPPER_CASE template parameters.
  [tools/hungarian_prefixes.py](tools/hungarian_prefixes.py) runs `clang-query` and
  checks that each prefix matches the declared or deduced type. Neither checks the
  domain words; review must. Do not add `NOLINT`, broad exclusions, or weaken either
  check to hide violations; extend the prefix table only with guardrail coverage.
- Keep `.clang-format`, `.clang-tidy`, this file, README, and CI consistent.
- Configure with tests and benchmarks enabled so the compilation database covers
  every first-party translation unit. Run
  `python3 tools/check_style.py --build-dir build` and
  `python3 tools/test_style_guardrails.py` when changing names or the checks.
  Missing tools, omitted sources, compiler errors, and violations must fail.
- Run `cmake --build build --parallel` and
  `ctest --test-dir build --output-on-failure`. Preserve all puzzle answers and
  verify naming-only changes do not alter algorithms. Use the README sanitizer
  build for algorithm or memory changes. Keep the dependency-free CLI working.
- CI runs tests, sanitizer tests, naming, and formatting on pushes and pull
  requests. Benchmark source is linted, but **benchmarks must not run in CI**.
  Actual timings require private inputs and the reference M4; retain measured
  results for naming-only changes. Document any installed Homebrew tools.
