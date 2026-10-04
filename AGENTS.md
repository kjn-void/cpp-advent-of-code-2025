# Project conventions

Use idiomatic C++20 and descriptive domain names on `main`. Refer to the original
[Advent of Code 2025 problems](https://adventofcode.com/2025) and the puzzle vocabulary
in [README.md](README.md). This branch uses ordinary C++ names; `hungarian` has its
own documented Apps Hungarian policy.

## Naming

- Use `snake_case` for variables, parameters, constants, functions, lambda names,
  and structured bindings. Use `PascalCase` for types and template type parameters.
  Private data members end in `_`; public record fields do not.
- Prefer the puzzle's nouns: `JunctionBox`, `Connection`, `MachineDefinition`,
  `PresentShape`, `TreeRegion`, `battery_banks_`, and `outputs_by_device_`.
- Name what a value means, including its units and relationships. Distinguish
  `button_count` from `button_index`, `squared_distance` from distance, and
  `pivot_row_by_column` from a column index. Do not call every value `data`, `n`,
  `result`, or `temp` when a meaningful domain name is available.
- Distinguish immutable requirements from changing search state:
  `joltage_requirements` / `remaining_joltage` and
  `present_counts` / `remaining_counts`.
- Use `first`/`last` for inclusive endpoints and `end` for an exclusive endpoint.
  Keep spatial `x`, `y`, `z` separate from grid rows/columns and collection indices.
- Keep clear algorithm terms such as pivot, parity, prefix sum, and union-find.
  Predicates describe a true condition; functions describe their action or result.
- Preserve required external names, C++ operators, `main(int argc, char** argv)`,
  GoogleTest/Benchmark macro-generated identifiers, CMake options, and Python
  framework hooks. Keep `DayNN` solver names and stable file/target/CLI names.
  Apply the same descriptive style to tests and project-owned tooling.

## Guardrails

- Keep `.clang-format`, `.clang-tidy`, this file, README, and CI consistent.
  Clang-tidy enforces spelling and private-member suffixes; review must verify
  semantic meaning. Do not add `NOLINT`, broad exclusions, or weaken the rules
  to hide violations.
- Configure with tests and benchmarks enabled to include every first-party C++
  translation unit in the compilation database. Run
  `python3 tools/check_style.py --build-dir build` and
  `python3 tools/test_style_guardrails.py` when changing naming or its checks.
  Missing tools, omitted source files, compiler errors, and violations must fail.
- Run `cmake --build build --parallel` and
  `ctest --test-dir build --output-on-failure`. Preserve all puzzle answers and
  verify naming-only changes do not alter algorithms. Use the README sanitizer
  build for algorithm or memory changes. Keep the dependency-free CLI working.
- CI runs tests, sanitizer tests, naming, and formatting on pushes and pull
  requests. Benchmark source is linted, but **benchmarks must not run in CI**.
  Actual timings require private inputs and the reference M4; retain measured
  results for naming-only changes. Document any installed Homebrew tools.
