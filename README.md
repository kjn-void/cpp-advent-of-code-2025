# Advent of Code 2025 — C++20

Solutions for days 1–12, with a command-line runner, GoogleTest tests, and Google
Benchmark benchmarks. Requires CMake 3.20 or newer and a C++20 compiler/standard library, including
`std::jthread`. With Apple's toolchain, use Xcode/Command Line Tools 26 or newer.

## Build and run

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/aoc2025 1 4 5 7
```

Run from the repository root. Save your puzzle inputs as `input/day01.txt` through
`input/day12.txt`. Inputs are read locally; automatic downloading is not implemented.
The input directory is ignored by Git. Both LF and CRLF files work, and blank lines
and significant spaces are preserved.

With a multi-configuration generator such as Visual Studio, build using
`cmake --build build --config Release --parallel` and run `build/Release/aoc2025.exe`.

The runner accepts one or more day numbers. Invalid arguments, missing inputs, and
solver errors produce a nonzero exit status; other requested days still run.
Day 12 returns `0` for part two: it has no second puzzle, and its star is awarded once
the other 23 stars are earned.

To build just the runner without GoogleTest or Google Benchmark:

```sh
cmake -S . -B build-cli -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=OFF -DAOC_BUILD_BENCHMARKS=OFF
cmake --build build-cli --parallel
```

## Tests and benchmarks

Both are enabled by default. CMake looks for each dependency in its vendored
folder (`googletest/` or `benchmark/`), then as an installed package, and finally
fetches a pinned release from GitHub. Disabling a target also skips its dependency.

```sh
ctest --test-dir build --output-on-failure
./build/benchmarks --benchmark_min_time=0.2s
```

Use `ctest --test-dir build -C Release --output-on-failure` for multi-configuration
builds. Benchmarks must run from the repository root so they can find the inputs.
They measure solver construction, parsing, and both parts; file I/O is excluded.
Wall-clock timing includes work performed by Day 10's worker threads. Missing
inputs are reported as benchmark errors.

Tests include the puzzle examples, empty inputs, malformed inputs, large values,
thread exception propagation, and deterministic comparisons with exhaustive
solvers on small generated problems.

For AddressSanitizer and UndefinedBehaviorSanitizer with Clang or GCC:

```sh
cmake -S . -B build-sanitize -DCMAKE_BUILD_TYPE=Debug \
  -DAOC_BUILD_BENCHMARKS=OFF \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer"
cmake --build build-sanitize --parallel
ctest --test-dir build-sanitize --output-on-failure
```

## Development tools and naming checks

Names follow the original [2025 puzzle descriptions](https://adventofcode.com/2025).
Use descriptive `snake_case` for variables and functions, `PascalCase` for types,
and a trailing underscore for private state. [AGENTS.md](AGENTS.md) defines the
review rules and exceptions. For example, `pivot_row_by_column` identifies both
sides of the mapping, while `remaining_counts` distinguishes the packing search
state from a region's requested `present_counts`.

On macOS, install the Xcode Command Line Tools (or select a full Xcode toolchain)
and the following Homebrew packages:

```sh
xcode-select --install  # only when no Apple development toolchain is installed
brew install cmake llvm clang-format python googletest google-benchmark
```

CMake drives builds; LLVM supplies `clang-tidy`; `clang-format` checks formatting;
Python 3 runs the validation scripts without pip dependencies. GoogleTest and
Google Benchmark supply the optional test and benchmark targets. The CLI itself
requires only a C++20 compiler and CMake. The style script finds Homebrew's keg-only
LLVM automatically and obtains Apple's SDK from `xcrun`. On other platforms,
provide `clang-tidy`, `clang-format`, and Python 3 on PATH (or use the script's
`--clang-tidy` and `--clang-format` options).

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON -DAOC_BUILD_BENCHMARKS=ON
python3 tools/test_style_guardrails.py
python3 tools/check_style.py --build-dir build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

For workflow maintenance, the optional Homebrew tools used here are `actionlint`
(to validate GitHub Actions YAML) and `gh` (to inspect remote CI runs after
`gh auth login`). Homebrew also installs `shellcheck` for actionlint's shell checks:

```sh
brew install actionlint gh
actionlint .github/workflows/ci.yml
```

Keep both optional targets enabled in the **lint build's** compilation database:
the checker visits every first-party translation unit, including benchmark source
and project headers. It fails on missing sources/tools, formatting violations,
invalid names, or compiler diagnostics. `.clang-tidy` checks declaration capitalization and private-member suffixes;
code review must still check that names match the puzzle and the actual values.
The guardrail tests exercise valid/invalid declarations and missing compilation
units. LLVM/clang-format 23.1.2 were used for local validation of this migration.

[CI](.github/workflows/ci.yml) runs on macOS 26 on every push and pull request.
This supplies the required C++20 thread library. It validates
formatting and naming, runs all unit/CLI tests in Release and with ASan/UBSan, and
checks the dependency-free CLI build. **CI neither builds nor runs the benchmark
executable**; it configures and lints its source only. Actual benchmarks require
the proper puzzle inputs and the reference M4 described below.

## Reference M4 benchmarks

Measured on **2026-10-03** on the reference **Apple M4 (10 CPU cores, 16 GiB RAM)**,
macOS 26.5.2, AppleClang 17.0.0, CMake 4.4.3, and Google Benchmark 1.9.5.
Both builds use `Release` and the same local puzzle inputs. Values are median
wall-clock microseconds over seven repetitions, with a minimum of 0.3 seconds per
repetition; lower is better. Input file I/O is excluded.

The baseline is commit `1e5fcf0`, rebuilt on the same machine with wall-clock
benchmark timing enabled. This matters for Day 10 because the calling thread's
CPU time excludes its workers. The cleanup column includes the new validation,
overflow checks, deterministic edge ordering, and cycle detection.

| Day | Baseline (µs) | Cleanup (µs) |
| --- | ------------: | -----------: |
| 01 | 747.9 | 49.4 |
| 02 | 7.8 | 8.4 |
| 03 | 60.7 | 64.6 |
| 04 | 331.6 | 344.0 |
| 05 | 38.6 | 27.1 |
| 06 | 195.5 | 143.3 |
| 07 | 23.1 | 23.5 |
| 08 | 20,866.7 | 22,699.6 |
| 09 | 15,118.5 | 564.0 |
| 10 | 37,012.1 | 4,006.3 |
| 11 | 221.2 | 267.9 |
| 12 | 500.9 | 258.1 |

Reproduce the cleanup measurement from the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel --target benchmarks
./build/benchmarks --benchmark_min_time=0.3s --benchmark_repetitions=7 \
  --benchmark_report_aggregates_only=true \
  --benchmark_out=build/benchmark-m4.json --benchmark_out_format=json
```

These are measurements for the saved puzzle inputs, not worst-case bounds.
The naming migration preserves the algorithms; this table retains the measured
cleanup results rather than presenting new timings for renamed code.

## Puzzle vocabulary

| Day and original problem | Names used in the implementation |
| --- | --- |
| [1: Secret Entrance](https://adventofcode.com/2025/day/1) | rotations, clicks, dial position, clicks that land on zero |
| [2: Gift Shop](https://adventofcode.com/2025/day/2) | product ID ranges, repeated digit blocks, invalid ID sum |
| [3: Lobby](https://adventofcode.com/2025/day/3) | battery banks, selected ratings, output joltage |
| [4: Printing Department](https://adventofcode.com/2025/day/4) | paper roll diagram, adjacent roll counts, accessible and removed rolls |
| [5: Cafeteria](https://adventofcode.com/2025/day/5) | fresh ID ranges, available ingredient IDs |
| [6: Trash Compactor](https://adventofcode.com/2025/day/6) | worksheet, problem columns, numbers, grand total |
| [7: Laboratories](https://adventofcode.com/2025/day/7) | tachyon manifold, beams, splitters, timelines |
| [8: Playground](https://adventofcode.com/2025/day/8) | junction boxes, box pairs by distance, connections, circuits |
| [9: Movie Theater](https://adventofcode.com/2025/day/9) | red and green tiles, rectangle area, compressed red tiles, boundary segments, forbidden (neither red nor green) cells |
| [10: Factory](https://adventofcode.com/2025/day/10) | indicator light diagram, button wirings, joltage counters and requirements, press counts |
| [11: Reactor](https://adventofcode.com/2025/day/11) | devices, outputs, paths from `you` / `svr` to `out`, required visits to `dac` and `fft` |
| [12: Christmas Tree Farm](https://adventofcode.com/2025/day/12) | present shapes, orientations, tree regions, present counts; part 2 is the 24th star |

Coordinates (`x`, `y`, `z`) remain distinct from grid rows/columns and sequence
indices. Inclusive endpoints use `first`/`last`; exclusive endpoints use `end`.
Algorithm terms such as pivot rows, parity, and prefix sums remain explicit where
they explain the implementation better than a story noun.

## Structure and conventions

- `src/core/`: solver interface, registry, input/parsing helpers, parallel reduction.
- `src/days/`: each day's parsing and algorithms.
- `src/main.cpp`: command-line runner.
- `tests/`: examples, regressions, and independent reference checks.
- `benchmarks/`: full-solver benchmarks.

Solvers implement `Solution::set_input`, `part1`, and `part2`. Calling either part
repeatedly is supported; `set_input` replaces the previous input. An object library
ensures the linker includes every day's static registration. Registration uses the
constrained `core::DayRegistration<DaySolver>` template.

The implementation uses standard C++20 facilities, including ranges algorithms,
`std::span` for non-owning helper parameters, `std::from_chars` for integer parsing,
and `std::jthread` for worker lifetimes. Worker exceptions are rethrown on the
calling thread. Formatting is defined in `.clang-format`.

Day 1 counts clicks that land on zero arithmetically. Day 9 uses coordinate compression and
a prefix sum of forbidden tiles to check rectangles exactly. Day 10 solves lights
over GF(2) and joltage using exact integer parity recursion: write each button's press count as
`odd + 2 * rest`, enumerate the odd presses, and solve the halved remaining target.
Memoization reuses repeated targets; there is no floating-point arithmetic.

Day 12 rejects insufficient area and accepts regions when disjoint bounding boxes
prove that all pieces fit. Other cases use exact packing with rotations and
reflections. Like general packing, difficult cases can require exponential search;
Day 10's subset enumeration is also exponential in the number of distinct buttons.
