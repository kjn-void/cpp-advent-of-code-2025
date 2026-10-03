#include <benchmark/benchmark.h>

#include "core/Input.h"
#include "core/Registry.h"
#include "core/Solution.h"

#include <exception>
#include <stdexcept>

// Input I/O is excluded; construction, parsing, and both parts are timed.
static void bench_day_full(benchmark::State& state, int day) {
    try {
        const auto lines = core::read_input(day);
        for (auto _ : state) {
            auto solver = Registry::instance().make(day);
            if (!solver)
                throw std::runtime_error("Day is not registered");
            solver->set_input(lines);
            benchmark::DoNotOptimize(solver->part1());
            benchmark::DoNotOptimize(solver->part2());
        }
    } catch (const std::exception& error) {
        state.SkipWithError(error.what());
    }
}

// Wall time includes work performed by Day 10's worker threads.
BENCHMARK_CAPTURE(bench_day_full, day01, 1)->UseRealTime();
BENCHMARK_CAPTURE(bench_day_full, day02, 2)->UseRealTime();
BENCHMARK_CAPTURE(bench_day_full, day03, 3)->UseRealTime();
BENCHMARK_CAPTURE(bench_day_full, day04, 4)->UseRealTime();
BENCHMARK_CAPTURE(bench_day_full, day05, 5)->UseRealTime();
BENCHMARK_CAPTURE(bench_day_full, day06, 6)->UseRealTime();
BENCHMARK_CAPTURE(bench_day_full, day07, 7)->UseRealTime();
BENCHMARK_CAPTURE(bench_day_full, day08, 8)->UseRealTime();
BENCHMARK_CAPTURE(bench_day_full, day09, 9)->UseRealTime();
BENCHMARK_CAPTURE(bench_day_full, day10, 10)->UseRealTime();
BENCHMARK_CAPTURE(bench_day_full, day11, 11)->UseRealTime();
BENCHMARK_CAPTURE(bench_day_full, day12, 12)->UseRealTime();
