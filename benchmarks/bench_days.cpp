#include <benchmark/benchmark.h>
#include <fstream>
#include <vector>
#include <string>

#include "core/Registry.h"
#include "core/Solution.h"   // ← REQUIRED

using namespace std;

// ------------------------------------------------------------
// Input loader (bench-only)
// ------------------------------------------------------------

static vector<string> read_input(int day) {
    char buf[64];
    snprintf(buf, sizeof(buf), "input/day%02d.txt", day);

    ifstream in(buf);
    if (!in) {
        throw runtime_error("Missing input file: " + string(buf));
    }

    vector<string> lines;
    string line;
    while (getline(in, line)) {
        lines.push_back(line);
    }
    return lines;
}

// ------------------------------------------------------------
// Benchmarks
// ------------------------------------------------------------

static void bench_day_full(benchmark::State& state, int day) {
    auto lines = read_input(day);

    for (auto _ : state) {
        auto solver = Registry::instance().make(day);
        solver->set_input(lines);
        benchmark::DoNotOptimize(solver->part1());
        benchmark::DoNotOptimize(solver->part2());
    }
}

#define BENCH_DAY(DAY) \
    static void bench_day_##DAY##_full(benchmark::State& state) { \
        bench_day_full(state, DAY); \
    } \
    BENCHMARK(bench_day_##DAY##_full);

BENCH_DAY(1)
BENCH_DAY(2)
BENCH_DAY(3)
BENCH_DAY(4)
BENCH_DAY(5)
BENCH_DAY(6)
BENCH_DAY(7)
BENCH_DAY(8)
BENCH_DAY(9)
BENCH_DAY(10)
BENCH_DAY(11)
BENCH_DAY(12)

BENCHMARK_MAIN();