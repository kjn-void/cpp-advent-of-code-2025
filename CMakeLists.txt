cmake_minimum_required(VERSION 3.20)
project(aoc2025 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

# ================================================================
# GoogleTest: vendored → system → FetchContent
# ================================================================

if (EXISTS "${CMAKE_SOURCE_DIR}/googletest/CMakeLists.txt")
    message(STATUS "Using vendored GoogleTest")
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    add_subdirectory(googletest)

else()
    find_package(GTest QUIET)

    if (GTest_FOUND)
        message(STATUS "Using system GoogleTest")
    else()
        message(STATUS "Fetching GoogleTest from GitHub (FetchContent)")

        include(FetchContent)

        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG main
        )

        set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

        FetchContent_MakeAvailable(googletest)
    endif()
endif()

# ---- Normalize GoogleTest targets ----
if (TARGET gtest AND NOT TARGET GTest::gtest)
    add_library(GTest::gtest ALIAS gtest)
endif()

if (TARGET gtest_main AND NOT TARGET GTest::gtest_main)
    add_library(GTest::gtest_main ALIAS gtest_main)
endif()

# ================================================================
# Google Benchmark: vendored → system → FetchContent
# ================================================================

if (EXISTS "${CMAKE_SOURCE_DIR}/benchmark/CMakeLists.txt")
    message(STATUS "Using vendored Google Benchmark")
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    add_subdirectory(benchmark)

else()
    find_package(benchmark QUIET)

    if (benchmark_FOUND)
        message(STATUS "Using system Google Benchmark")
    else()
        message(STATUS "Fetching Google Benchmark from GitHub (FetchContent)")

        include(FetchContent)

        FetchContent_Declare(
            benchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG v1.8.3
        )

        set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
        set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON CACHE BOOL "" FORCE)

        FetchContent_MakeAvailable(benchmark)
    endif()
endif()

# ---- Normalize Google Benchmark targets ----
if (TARGET benchmark AND NOT TARGET benchmark::benchmark)
    add_library(benchmark::benchmark ALIAS benchmark)
endif()

if (TARGET benchmark_main AND NOT TARGET benchmark::benchmark_main)
    add_library(benchmark::benchmark_main ALIAS benchmark_main)
endif()

# ================================================================
# OBJECT library (static registration must live here)
# ================================================================

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

target_include_directories(aoc2025_obj PRIVATE src)

# ================================================================
# Main executable
# ================================================================

add_executable(aoc2025
    src/main.cpp
    $<TARGET_OBJECTS:aoc2025_obj>
)

target_include_directories(aoc2025 PRIVATE src)

# ================================================================
# Unit tests
# ================================================================

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

# ================================================================
# Benchmarks
# ================================================================

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