cmake_minimum_required(VERSION 3.20)
project(aoc2025 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
option(AOC_BUILD_BENCHMARKS "Build Google Benchmark benchmarks" ON)
find_package(Threads REQUIRED)

# ================================================================
# GoogleTest: vendored → system → FetchContent
# ================================================================

if (BUILD_TESTING)
    if (MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "Use the shared MSVC runtime" FORCE)
    endif()
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt")
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
                GIT_TAG v1.17.0
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

endif()

# ================================================================
# Google Benchmark: vendored → system → FetchContent
# ================================================================

if (AOC_BUILD_BENCHMARKS)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/benchmark/CMakeLists.txt")
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
                GIT_TAG v1.9.5
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

endif()

# ================================================================
# OBJECT library (static registration must live here)
# ================================================================

add_library(aoc2025_obj OBJECT
    src/core/Input.cpp
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

target_include_directories(aoc2025_obj PUBLIC src)
target_compile_features(aoc2025_obj PUBLIC cxx_std_20)
target_link_libraries(aoc2025_obj PUBLIC Threads::Threads)
if (MSVC)
    target_compile_options(aoc2025_obj PRIVATE /W4)
else()
    target_compile_options(aoc2025_obj PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ================================================================
# Main executable
# ================================================================

add_executable(aoc2025
    src/main.cpp
)

target_link_libraries(aoc2025 PRIVATE aoc2025_obj)

# ================================================================
# Unit tests
# ================================================================

if (BUILD_TESTING)
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
        tests/test_core.cpp
        tests/test_regressions.cpp
        tests/test_utils.cpp
    )

    target_link_libraries(tests
        PRIVATE
            aoc2025_obj
            GTest::gtest_main
    )

    add_test(NAME aoc2025_tests COMMAND tests)
    set_tests_properties(aoc2025_tests PROPERTIES TIMEOUT 120)
    add_test(NAME aoc2025_cli
        COMMAND ${CMAKE_COMMAND} -DAOC_EXECUTABLE=$<TARGET_FILE:aoc2025>
            -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cli.cmake)
    set_tests_properties(aoc2025_cli PROPERTIES TIMEOUT 10)

endif()

# ================================================================
# Benchmarks
# ================================================================

if (AOC_BUILD_BENCHMARKS)
    add_executable(benchmarks
        benchmarks/bench_days.cpp
    )

    target_link_libraries(benchmarks
        PRIVATE
            aoc2025_obj
            benchmark::benchmark_main
    )
endif()
