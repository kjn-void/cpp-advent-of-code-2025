// src/core/Parallel.h
#pragma once

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <numeric>
#include <thread>
#include <vector>

namespace core {

// Parallel sum over indices [0..n), using a worker-count-sized partial reduction.
// Portable: works with AppleClang + Command Line Tools.
template <typename F> std::int64_t parallel_sum_indexed(std::size_t n, F&& fn) {
    if (n == 0)
        return 0;

    const unsigned hc = std::max(1u, std::thread::hardware_concurrency());
    const auto workers = std::min<std::size_t>(hc, n);
    if (workers == 1) {
        std::int64_t total = 0;
        for (std::size_t i = 0; i < n; ++i)
            total += fn(i);
        return total;
    }

    std::atomic_size_t next{0};
    std::vector<std::int64_t> partial(workers, 0);

    std::vector<std::exception_ptr> errors(workers);
    std::atomic_bool failed{false};
    std::vector<std::jthread> threads;
    threads.reserve(workers);

    for (std::size_t t = 0; t < workers; ++t) {
        threads.emplace_back([&, t] {
            try {
                std::int64_t local = 0;
                while (!failed.load(std::memory_order_relaxed)) {
                    const auto i = next.fetch_add(1, std::memory_order_relaxed);
                    if (i >= n)
                        break;
                    local += fn(i);
                }
                partial[t] = local;
            } catch (...) {
                errors[t] = std::current_exception();
                failed.store(true, std::memory_order_relaxed);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    for (const auto& error : errors) {
        if (error)
            std::rethrow_exception(error);
    }
    return std::accumulate(partial.begin(), partial.end(), std::int64_t{0});
}

} // namespace core
