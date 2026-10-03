# Advent of Code 2025 — C++20

Solutions for days 1–12, with a command-line runner, GoogleTest tests, and Google
Benchmark benchmarks. Requires CMake 3.20 or newer and a C++20 compiler.

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
Day 12 returns `0` for part two because it has no second computational puzzle.

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

## Structure and conventions

- `src/core/`: solver interface, registry, input/parsing helpers, parallel reduction.
- `src/days/`: each day's parsing and algorithms.
- `src/main.cpp`: command-line runner.
- `tests/`: examples, regressions, and independent reference checks.
- `benchmarks/`: full-solver benchmarks.

Solvers implement `Solution::set_input`, `part1`, and `part2`. Calling either part
repeatedly is supported; `set_input` replaces the previous input. An object library
ensures the linker includes every day's static registration. Registration uses the
constrained `core::DayRegistration<Day>` template.

The implementation uses standard C++20 facilities, including ranges algorithms,
`std::span` for non-owning helper parameters, `std::from_chars` for integer parsing,
and `std::jthread` for worker lifetimes. Worker exceptions are rethrown on the
calling thread. Formatting is defined in `.clang-format`.

Day 1 counts dial crossings arithmetically. Day 9 uses coordinate compression and
a prefix sum of forbidden tiles to check rectangles exactly. Day 10 solves lights
over GF(2) and joltage using exact integer parity recursion: write each button's press count as
`odd + 2 * rest`, enumerate the odd presses, and solve the halved remaining target.
Memoization reuses repeated targets; there is no floating-point arithmetic.

Day 12 rejects insufficient area and accepts regions when disjoint bounding boxes
prove that all pieces fit. Other cases use exact packing with rotations and
reflections. Like general packing, difficult cases can require exponential search;
Day 10's subset enumeration is also exponential in the number of distinct buttons.
