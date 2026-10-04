#include "days/day07.h"
#include "core/Register.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>

// Registration
namespace {
const core::Drg<Day07> drgDay{7};
} // namespace

// ------------------------------------------------------------

void Day07::SetInput(const std::vector<std::string>& rgusLines) {
    gridManifold_ = rgusLines;
    crw_ = static_cast<int>(gridManifold_.size());
    ccol_ = 0;
    colStart_ = -1;
    for (const auto& usRow : gridManifold_)
        ccol_ = std::max(ccol_, static_cast<int>(usRow.size()));
    if (ccol_ == 0)
        return;
    for (auto& usRow : gridManifold_)
        usRow.resize(ccol_, '.');
    const auto colStart = gridManifold_.front().find('S');
    if (colStart == std::string::npos ||
        gridManifold_.front().find('S', colStart + 1) != std::string::npos)
        throw std::invalid_argument("Tachyon manifold must have one start in its first row");
    colStart_ = static_cast<int>(colStart);
}

// ------------------------------------------------------------
// Part 1 — count splits
// ------------------------------------------------------------

std::string Day07::TxtPart1() {
    if (colStart_ < 0)
        return "0";
    std::vector<bool> mpcolfBeamA(ccol_, false);
    std::vector<bool> mpcolfBeamB(ccol_, false);

    auto* pmpcolfActive = &mpcolfBeamA;
    auto* pmpcolfNext = &mpcolfBeamB;

    (*pmpcolfActive)[colStart_] = true;

    int cntSplits = 0;

    for (int rw = 1; rw < crw_; ++rw) {
        std::fill(pmpcolfNext->begin(), pmpcolfNext->end(), false);
        const auto& usRow = gridManifold_[rw];

        for (int col = 0; col < ccol_; ++col) {
            if (!(*pmpcolfActive)[col])
                continue;

            if (usRow[col] == '^') {
                cntSplits++;
                if (col > 0)
                    (*pmpcolfNext)[col - 1] = true;
                if (col + 1 < ccol_)
                    (*pmpcolfNext)[col + 1] = true;
            } else {
                (*pmpcolfNext)[col] = true;
            }
        }

        std::swap(pmpcolfActive, pmpcolfNext);
    }

    return std::to_string(cntSplits);
}

// ------------------------------------------------------------
// Part 2 — count timelines
// ------------------------------------------------------------

std::string Day07::TxtPart2() {
    if (colStart_ < 0)
        return "0";
    std::int64_t cntExitedTimelines = 0;
    std::vector<std::int64_t> mpcolcntTimelinesA(ccol_, 0);
    std::vector<std::int64_t> mpcolcntTimelinesB(ccol_, 0);

    auto* pmpcolcntActive = &mpcolcntTimelinesA;
    auto* pmpcolcntNext = &mpcolcntTimelinesB;

    (*pmpcolcntActive)[colStart_] = 1;

    for (int rw = 1; rw < crw_; ++rw) {
        std::fill(pmpcolcntNext->begin(), pmpcolcntNext->end(), 0);
        const auto& usRow = gridManifold_[rw];

        for (int col = 0; col < ccol_; ++col) {
            std::int64_t cntTimelines = (*pmpcolcntActive)[col];
            if (cntTimelines == 0)
                continue;

            if (usRow[col] == '^') {
                if (col > 0)
                    (*pmpcolcntNext)[col - 1] += cntTimelines;
                else
                    cntExitedTimelines += cntTimelines;
                if (col + 1 < ccol_)
                    (*pmpcolcntNext)[col + 1] += cntTimelines;
                else
                    cntExitedTimelines += cntTimelines;
            } else {
                (*pmpcolcntNext)[col] += cntTimelines;
            }
        }

        std::swap(pmpcolcntActive, pmpcolcntNext);
    }

    std::int64_t cntSurvivingTimelines =
        std::accumulate(pmpcolcntActive->begin(), pmpcolcntActive->end(), std::int64_t{0});

    return std::to_string(cntSurvivingTimelines + cntExitedTimelines);
}
