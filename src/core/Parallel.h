// src/core/Parallel.h
#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <thread>
#include <vector>
#include <algorithm>

namespace core {

// Parallel sum over indices [0..n), using a worker-count-sized partial reduction.
// Portable: works with AppleClang + Command Line Tools.
template <typename F>
int64_t parallel_sum_indexed(std::size_t n, F&& fn) {
    if (n == 0) return 0;

    const unsigned hc = std::max(1u, std::thread::hardware_concurrency());
    const unsigned workers = std::min<unsigned>(hc, static_cast<unsigned>(n));

    std::atomic_size_t next{0};
    std::vector<int64_t> partial(workers, 0);

    std::vector<std::thread> threads;
    threads.reserve(workers);

    for (unsigned t = 0; t < workers; ++t) {
        threads.emplace_back([&, t] {
            int64_t local = 0;
            for (;;) {
                const std::size_t i =
                    next.fetch_add(1, std::memory_order_relaxed);
                if (i >= n) break;
                local += fn(i);
            }
            partial[t] = local;
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    int64_t total = 0;
    for (auto v : partial) total += v;
    return total;
}

} // namespace core