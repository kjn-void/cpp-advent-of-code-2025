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

// Parallel sum over indices [0, item_count), using a worker-count-sized partial reduction.
// Portable: works with AppleClang + Command Line Tools.
template <typename Function>
std::int64_t parallel_sum_indexed(std::size_t item_count, Function&& value_at) {
    if (item_count == 0)
        return 0;

    const unsigned hardware_workers = std::max(1u, std::thread::hardware_concurrency());
    const auto worker_count = std::min<std::size_t>(hardware_workers, item_count);
    if (worker_count == 1) {
        std::int64_t total = 0;
        for (std::size_t item_index = 0; item_index < item_count; ++item_index)
            total += value_at(item_index);
        return total;
    }

    std::atomic_size_t next_item_index{0};
    std::vector<std::int64_t> worker_sums(worker_count, 0);

    std::vector<std::exception_ptr> worker_errors(worker_count);
    std::atomic_bool failed{false};
    std::vector<std::jthread> workers;
    workers.reserve(worker_count);

    for (std::size_t worker_index = 0; worker_index < worker_count; ++worker_index) {
        workers.emplace_back([&, worker_index] {
            try {
                std::int64_t worker_sum = 0;
                while (!failed.load(std::memory_order_relaxed)) {
                    const auto item_index = next_item_index.fetch_add(1, std::memory_order_relaxed);
                    if (item_index >= item_count)
                        break;
                    worker_sum += value_at(item_index);
                }
                worker_sums[worker_index] = worker_sum;
            } catch (...) {
                worker_errors[worker_index] = std::current_exception();
                failed.store(true, std::memory_order_relaxed);
            }
        });
    }

    for (auto& worker : workers) {
        worker.join();
    }

    for (const auto& worker_error : worker_errors) {
        if (worker_error)
            std::rethrow_exception(worker_error);
    }
    return std::accumulate(worker_sums.begin(), worker_sums.end(), std::int64_t{0});
}

} // namespace core
