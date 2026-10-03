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
template <typename Fn> std::int64_t ValSumIndexed(std::size_t citem, Fn&& fnValueAt) {
    if (citem == 0)
        return 0;

    const unsigned cwkrHardware = std::max(1u, std::thread::hardware_concurrency());
    const auto cwkr = std::min<std::size_t>(cwkrHardware, citem);
    if (cwkr == 1) {
        std::int64_t valSum = 0;
        for (std::size_t iitem = 0; iitem < citem; ++iitem)
            valSum += fnValueAt(iitem);
        return valSum;
    }

    std::atomic_size_t iitemNext{0};
    std::vector<std::int64_t> mpiwkrvalSum(cwkr, 0);

    std::vector<std::exception_ptr> mpiwkrerr(cwkr);
    std::atomic_bool fFailed{false};
    std::vector<std::jthread> rgwkr;
    rgwkr.reserve(cwkr);

    for (std::size_t iwkr = 0; iwkr < cwkr; ++iwkr) {
        rgwkr.emplace_back([&, iwkr] {
            try {
                std::int64_t valWorkerSum = 0;
                while (!fFailed.load(std::memory_order_relaxed)) {
                    const auto iitem = iitemNext.fetch_add(1, std::memory_order_relaxed);
                    if (iitem >= citem)
                        break;
                    valWorkerSum += fnValueAt(iitem);
                }
                mpiwkrvalSum[iwkr] = valWorkerSum;
            } catch (...) {
                mpiwkrerr[iwkr] = std::current_exception();
                fFailed.store(true, std::memory_order_relaxed);
            }
        });
    }

    for (auto& wkr : rgwkr) {
        wkr.join();
    }

    for (const auto& errWorker : mpiwkrerr) {
        if (errWorker)
            std::rethrow_exception(errWorker);
    }
    return std::accumulate(mpiwkrvalSum.begin(), mpiwkrvalSum.end(), std::int64_t{0});
}

} // namespace core
