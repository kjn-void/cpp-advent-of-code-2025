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
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
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

## 🧪 Tests (GoogleTest)

This project supports **three ways** of using GoogleTest, in the following priority order:

### 1️⃣ Vendored GoogleTest (preferred)

If a `googletest/` directory exists at the repository root, it is used automatically.

```
cpp-advent-of-code-2025/
├── googletest/
│   ├── CMakeLists.txt
│   └── …
```

This is the **recommended setup** for:
- reproducible builds
- CI environments
- avoiding system / distro quirks

To add GoogleTest as a submodule:

```bash
git submodule add https://github.com/google/googletest.git googletest
git submodule update --init --recursive
```

### 2️⃣ System-installed GoogleTest (fallback)

If googletest/ is not present, CMake falls back to a system-installed GoogleTest via:

```cmake
find_package(GTest REQUIRED)
```

On Ubuntu, this typically requires:

```sh
sudo apt install libgtest-dev
cd /usr/src/googletest
sudo cmake -B build
sudo cmake --build build
sudo cp build/lib/*.a /usr/lib/
```

Note: Ubuntu ships GoogleTest as source-only, so the libraries must be built manually.

### 3️⃣ GitHub GoogleTest via FetchContent (alternative)

If you prefer not to vendor GoogleTest as a submodule, you can also fetch it directly from GitHub using CMake’s FetchContent.

This is **not enabled by default**, but can be added easily if desired.

Typical configuration:

```cmake
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.14.0
)

FetchContent_MakeAvailable(googletest)
```

This approach:

* avoids system installs
* avoids committing GoogleTest to your repo
* works well in CI

## 🏃 Running the tests

Once built:

```sh
./build/tests
```

or, if CTest integration is enabled:

```sh
ctest --test-dir build
```

## ⏱️ Benchmarks

If you use Google Benchmark:

```bash
./build/benchmarks
```

Tip: run once to ensure inputs are cached locally before benchmarking.

## 📊 Benchmark Summary — Apple M4 (darwin/arm64)

| Day | SetInput (µs) | Part1 (µs) | Part2 (µs) | Full (µs) |
| --- | ------------- | ---------- | ---------- | --------- |
| 01  |               |            |            | 732       |
| 02  |               |            |            | 8         |
| 03  |               |            |            | 60        |
| 04  |               |            |            | 349       |
| 05  |               |            |            | 37        |
| 06  |               |            |            | 184       |
| 07  |               |            |            | 25        |
| 08  |               |            |            | 21_000    |
| 09  |               |            |            | 14_000    |
| 10  |               |            |            | 39_000    |
| 11  |               |            |            | 227       |
| 12  |               |            |            | 582       |
