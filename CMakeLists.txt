cmake_minimum_required(VERSION 3.20)
project(aoc2025 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)

enable_testing()

# ------------------------------------------------------------------
# OBJECT library (static registration must live here)
# ------------------------------------------------------------------

add_library(aoc2025_obj OBJECT
    src/core/Registry.cpp
    src/days/day01.cpp
    src/days/day02.cpp
    src/days/day03.cpp
    src/days/day04.cpp
    src/days/day05.cpp
    src/days/day06.cpp
    src/days/day07.cpp
    src/days/day08.cpp
    src/days/day09.cpp
    src/days/day10.cpp
    src/days/day11.cpp
    src/days/day12.cpp
)

# ✅ REQUIRED so day01.cpp can include "core/Solution.h"
target_include_directories(aoc2025_obj PRIVATE src)

# ------------------------------------------------------------------
# Main executable
# ------------------------------------------------------------------

add_executable(aoc2025
    src/main.cpp
    $<TARGET_OBJECTS:aoc2025_obj>
)

target_include_directories(aoc2025 PRIVATE src)

# ------------------------------------------------------------------
# Unit tests
# ------------------------------------------------------------------

add_executable(tests
    tests/test_day01.cpp
    tests/test_day02.cpp
    tests/test_day03.cpp
    tests/test_day04.cpp
    tests/test_day05.cpp
    tests/test_day06.cpp
    tests/test_day07.cpp
    tests/test_day08.cpp
    tests/test_day09.cpp
    tests/test_day10.cpp
    tests/test_day11.cpp
    tests/test_day12.cpp
    tests/test_utils.cpp
    $<TARGET_OBJECTS:aoc2025_obj>
)

target_include_directories(tests PRIVATE src)

target_link_libraries(tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
)

add_test(NAME aoc2025_tests COMMAND tests)

# ------------------------------------------------------------------
# Benchmarks
# ------------------------------------------------------------------

add_executable(benchmarks
    benchmarks/bench_days.cpp
    $<TARGET_OBJECTS:aoc2025_obj>
)

target_include_directories(benchmarks PRIVATE src)

target_link_libraries(benchmarks
    PRIVATE
        benchmark::benchmark
        benchmark::benchmark_main
)