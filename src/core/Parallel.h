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

// Parallel sum over indices [0, uItemCount), using a worker-count-sized partial reduction.
// Portable: works with AppleClang + Command Line Tools.
template <typename FUNCTION>
std::int64_t ParallelSumIndexed(std::size_t uItemCount, FUNCTION&& functionValueAt) {
    if (uItemCount == 0)
        return 0;

    const unsigned uHardwareWorkers = std::max(1u, std::thread::hardware_concurrency());
    const auto uWorkerCount = std::min<std::size_t>(uHardwareWorkers, uItemCount);
    if (uWorkerCount == 1) {
        std::int64_t iTotal = 0;
        for (std::size_t uItemIndex = 0; uItemIndex < uItemCount; ++uItemIndex)
            iTotal += functionValueAt(uItemIndex);
        return iTotal;
    }

    std::atomic_size_t atomicNextItemIndex{0};
    std::vector<std::int64_t> vectorWorkerSums(uWorkerCount, 0);

    std::vector<std::exception_ptr> vectorWorkerErrors(uWorkerCount);
    std::atomic_bool atomicFailed{false};
    std::vector<std::jthread> vectorWorkers;
    vectorWorkers.reserve(uWorkerCount);

    for (std::size_t uWorkerIndex = 0; uWorkerIndex < uWorkerCount; ++uWorkerIndex) {
        vectorWorkers.emplace_back([&, uWorkerIndex] {
            try {
                std::int64_t iWorkerSum = 0;
                while (!atomicFailed.load(std::memory_order_relaxed)) {
                    const auto uItemIndex =
                        atomicNextItemIndex.fetch_add(1, std::memory_order_relaxed);
                    if (uItemIndex >= uItemCount)
                        break;
                    iWorkerSum += functionValueAt(uItemIndex);
                }
                vectorWorkerSums[uWorkerIndex] = iWorkerSum;
            } catch (...) {
                vectorWorkerErrors[uWorkerIndex] = std::current_exception();
                atomicFailed.store(true, std::memory_order_relaxed);
            }
        });
    }

    for (auto& jthreadWorker : vectorWorkers) {
        jthreadWorker.join();
    }

    for (const auto& exceptionptrWorkerError : vectorWorkerErrors) {
        if (exceptionptrWorkerError)
            std::rethrow_exception(exceptionptrWorkerError);
    }
    return std::accumulate(vectorWorkerSums.begin(), vectorWorkerSums.end(), std::int64_t{0});
}

} // namespace core
