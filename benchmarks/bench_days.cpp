#include <benchmark/benchmark.h>

#include "core/Input.h"
#include "core/Registry.h"
#include "core/Solution.h"

#include <exception>
#include <stdexcept>

// Input I/O is excluded; construction, parsing, and both parts are timed.
static void BenchDayFull(benchmark::State& bms, int idDay) {
    try {
        const auto rgusLines = core::RgusReadInput(idDay);
        for (auto iterBenchmark : bms) {
            auto pslvDay = Regy::RegyInstance().PslvMake(idDay);
            if (!pslvDay)
                throw std::runtime_error("Day is not registered");
            pslvDay->SetInput(rgusLines);
            benchmark::DoNotOptimize(pslvDay->TxtPart1());
            benchmark::DoNotOptimize(pslvDay->TxtPart2());
        }
    } catch (const std::exception& errFailure) {
        bms.SkipWithError(errFailure.what());
    }
}

// Wall time includes work performed by Day 10's worker threads.
BENCHMARK_CAPTURE(BenchDayFull, day01, 1)->UseRealTime();
BENCHMARK_CAPTURE(BenchDayFull, day02, 2)->UseRealTime();
BENCHMARK_CAPTURE(BenchDayFull, day03, 3)->UseRealTime();
BENCHMARK_CAPTURE(BenchDayFull, day04, 4)->UseRealTime();
BENCHMARK_CAPTURE(BenchDayFull, day05, 5)->UseRealTime();
BENCHMARK_CAPTURE(BenchDayFull, day06, 6)->UseRealTime();
BENCHMARK_CAPTURE(BenchDayFull, day07, 7)->UseRealTime();
BENCHMARK_CAPTURE(BenchDayFull, day08, 8)->UseRealTime();
BENCHMARK_CAPTURE(BenchDayFull, day09, 9)->UseRealTime();
BENCHMARK_CAPTURE(BenchDayFull, day10, 10)->UseRealTime();
BENCHMARK_CAPTURE(BenchDayFull, day11, 11)->UseRealTime();
BENCHMARK_CAPTURE(BenchDayFull, day12, 12)->UseRealTime();
