# Advent of Code 2025 (C++)

This repository contains a clean, extensible **C++ framework** for solving **Advent of Code 2025** using a small virtual interface:

```cpp
struct Solution {
    virtual ~Solution() = default;

    // Called once per run/day with the raw input lines.
    virtual void set_input(const std::vector<std::string>& lines) = 0;

    // Return answers as strings to keep printing/benchmarking simple.
    virtual std::string part1() = 0;
    virtual std::string part2() = 0;
};
```

Each day’s solution implements this interface and registers itself in a central registry.

## 📦 Project Structure

```
cpp-advent-of-code-2025/
│
├── README.md
├── CMakeLists.txt
│
├── src/
│   ├── main.cpp                # CLI entry point
│   ├── core/
│   │   ├── Solution.h          # Solution interface
│   │   ├── Registry.h/.cpp     # Day registration + factory
│   │   └── Parallel.h          # tiny helper for parallel reductions
│   └── days/
│       ├── day01.h/.cpp
│       ├── ...
│       └── day12.h/.cpp
│
├── input/                      # cached input files (auto-created / gitignored)
│   └── (empty until downloaded)
│
└── tests/
    ├── test_day01.cpp
    ├── ...
    └── test_day12.cpp
```

> Note: exact filenames may differ slightly depending on your layout, but the concepts are the same: `core/` + `days/` + `tests/` + `input/`.

## 🚀 Building & Running

Configure + build:

```bash
cmake -S . -B build
cmake --build build
```

Run a single day:

```bash
./build/aoc2025 1
```

Run multiple days:

```bash
./build/aoc2025 1 4 5
```

The CLI accepts any number of days (e.g. 1–25 if you implement more).

## 🌐 Automatic Input Download From adventofcode.com (optional)

If you implement input downloading, you typically fetch:

```
https://adventofcode.com/2025/day/<DAY>/input
```

Advent of Code does not provide OAuth or an official API login. Authentication is via a cookie named:

```
session=YOUR_SESSION_TOKEN
```

⚠️ This token is your personal authentication. **Do not commit it** and do not share it.

## 🔑 How to Retrieve Your Session Token

1. Log in to https://adventofcode.com/
2. Open your browser’s Developer Tools
   - Safari: ⌥ Option + ⌘ Command + I  
   - Chrome: F12 → Application tab  
   - Firefox: F12 → Storage tab
3. Go to Cookies → https://adventofcode.com
4. Look for a cookie named: `session`
5. Copy the value (a long hex-like string)

## 🧷 Enabling Automatic Download

A common pattern is to support environment variables such as:

```bash
export AOC_SESSION="your-session-token"
export AOC_ONLINE=1
```

Then when you run:

```bash
./build/aoc2025 1
```

the program can:
1. attempt to download the input
2. save it to `input/day01.txt`
3. read from disk on subsequent runs (fallback if offline)

(Exact env var names and behavior depend on your implementation.)

## 🧪 Tests

If you use GoogleTest:

```bash
./build/tests
```

## ⏱️ Benchmarks

If you use Google Benchmark:

```bash
./build/benchmarks
```

Tip: run once to ensure inputs are cached locally before benchmarking.
