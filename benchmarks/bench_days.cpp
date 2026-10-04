#include <benchmark/benchmark.h>

#include "core/Input.h"
#include "core/Registry.h"
#include "core/Solution.h"

#include <exception>
#include <stdexcept>

// Input I/O is excluded; construction, parsing, and both parts are timed.
static void benchmark_day(benchmark::State& benchmark_state, int day) {
    try {
        const auto input_lines = core::read_input(day);
        for (auto iteration : benchmark_state) {
            auto solver = Registry::instance().make(day);
            if (!solver)
                throw std::runtime_error("Day is not registered");
            solver->set_input(input_lines);
            benchmark::DoNotOptimize(solver->part1());
            benchmark::DoNotOptimize(solver->part2());
        }
    } catch (const std::exception& error) {
        benchmark_state.SkipWithError(error.what());
    }
}

// Wall time includes work performed by Day 10's worker threads.
BENCHMARK_CAPTURE(benchmark_day, day01, 1)->UseRealTime();
BENCHMARK_CAPTURE(benchmark_day, day02, 2)->UseRealTime();
BENCHMARK_CAPTURE(benchmark_day, day03, 3)->UseRealTime();
BENCHMARK_CAPTURE(benchmark_day, day04, 4)->UseRealTime();
BENCHMARK_CAPTURE(benchmark_day, day05, 5)->UseRealTime();
BENCHMARK_CAPTURE(benchmark_day, day06, 6)->UseRealTime();
BENCHMARK_CAPTURE(benchmark_day, day07, 7)->UseRealTime();
BENCHMARK_CAPTURE(benchmark_day, day08, 8)->UseRealTime();
BENCHMARK_CAPTURE(benchmark_day, day09, 9)->UseRealTime();
BENCHMARK_CAPTURE(benchmark_day, day10, 10)->UseRealTime();
BENCHMARK_CAPTURE(benchmark_day, day11, 11)->UseRealTime();
BENCHMARK_CAPTURE(benchmark_day, day12, 12)->UseRealTime();
