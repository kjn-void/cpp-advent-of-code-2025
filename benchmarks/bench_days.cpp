#include <benchmark/benchmark.h>

#include "core/Input.h"
#include "core/Registry.h"
#include "core/Solution.h"

#include <exception>
#include <stdexcept>

// Input I/O is excluded; construction, parsing, and both parts are timed.
static void BenchmarkDay(benchmark::State& stateBenchmark, int iDay) {
    try {
        const auto vectorInputLines = core::ReadInput(iDay);
        for (auto iteration_ : stateBenchmark) {
            auto psolution = Registry::Instance().Make(iDay);
            if (!psolution)
                throw std::runtime_error("Day is not registered");
            psolution->SetInput(vectorInputLines);
            benchmark::DoNotOptimize(psolution->Part1());
            benchmark::DoNotOptimize(psolution->Part2());
        }
    } catch (const std::exception& exceptionError) {
        stateBenchmark.SkipWithError(exceptionError.what());
    }
}

// Wall time includes work performed by Day 10's worker threads.
BENCHMARK_CAPTURE(BenchmarkDay, day01, 1)->UseRealTime();
BENCHMARK_CAPTURE(BenchmarkDay, day02, 2)->UseRealTime();
BENCHMARK_CAPTURE(BenchmarkDay, day03, 3)->UseRealTime();
BENCHMARK_CAPTURE(BenchmarkDay, day04, 4)->UseRealTime();
BENCHMARK_CAPTURE(BenchmarkDay, day05, 5)->UseRealTime();
BENCHMARK_CAPTURE(BenchmarkDay, day06, 6)->UseRealTime();
BENCHMARK_CAPTURE(BenchmarkDay, day07, 7)->UseRealTime();
BENCHMARK_CAPTURE(BenchmarkDay, day08, 8)->UseRealTime();
BENCHMARK_CAPTURE(BenchmarkDay, day09, 9)->UseRealTime();
BENCHMARK_CAPTURE(BenchmarkDay, day10, 10)->UseRealTime();
BENCHMARK_CAPTURE(BenchmarkDay, day11, 11)->UseRealTime();
BENCHMARK_CAPTURE(BenchmarkDay, day12, 12)->UseRealTime();
