#include "days/day05.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <limits>
#include <stdexcept>

// ------------------------------------------------------------
// Registration
// ------------------------------------------------------------
namespace {
const core::DayRegistration<Day05> dayregistration{5};
} // namespace

// ------------------------------------------------------------

void Day05::SetInput(const std::vector<std::string>& vectorInputLines) {
    m_vectorFreshIdRanges.clear();
    m_vectorAvailableIngredientIds.clear();

    int iSectionIndex = 0;

    for (const auto& stringRawLine : vectorInputLines) {
        const auto stringLine = core::Trim(stringRawLine);
        if (stringLine.empty()) {
            ++iSectionIndex;
            continue;
        }

        if (iSectionIndex == 0) {
            // range
            auto uDashOffset = stringLine.find('-');
            if (uDashOffset == std::string_view::npos)
                throw std::invalid_argument("Expected a fresh ID range");
            const auto iFirstId =
                core::ParseInteger<std::int64_t>(stringLine.substr(0, uDashOffset));
            const auto iLastId =
                core::ParseInteger<std::int64_t>(stringLine.substr(uDashOffset + 1));
            if (iFirstId < 0 || iLastId < iFirstId)
                throw std::invalid_argument("Invalid fresh ID range");
            m_vectorFreshIdRanges.emplace_back(iFirstId, iLastId);
        } else {
            // id
            m_vectorAvailableIngredientIds.push_back(core::ParseInteger<std::int64_t>(stringLine));
        }
    }

    // merge overlapping ranges
    std::ranges::sort(m_vectorFreshIdRanges);
    if (m_vectorFreshIdRanges.empty())
        return;

    std::vector<std::pair<std::int64_t, std::int64_t>> vectorMergedRanges;
    std::int64_t iMergedFirstId = m_vectorFreshIdRanges[0].first;
    std::int64_t iMergedLastId = m_vectorFreshIdRanges[0].second;

    for (std::size_t uRangeIndex = 1; uRangeIndex < m_vectorFreshIdRanges.size(); ++uRangeIndex) {
        auto [iFirstId, iLastId] = m_vectorFreshIdRanges[uRangeIndex];
        if (iFirstId <= iMergedLastId) {
            iMergedLastId = std::max(iMergedLastId, iLastId);
        } else {
            vectorMergedRanges.emplace_back(iMergedFirstId, iMergedLastId);
            iMergedFirstId = iFirstId;
            iMergedLastId = iLastId;
        }
    }
    vectorMergedRanges.emplace_back(iMergedFirstId, iMergedLastId);
    m_vectorFreshIdRanges.swap(vectorMergedRanges);
}

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

bool Day05::IsFresh(std::int64_t iIngredientId) const {
    // binary search in merged ranges
    int iFirstRangeIndex = 0;
    int iLastRangeIndex = static_cast<int>(m_vectorFreshIdRanges.size()) - 1;

    while (iFirstRangeIndex <= iLastRangeIndex) {
        int iMiddleRangeIndex = (iFirstRangeIndex + iLastRangeIndex) / 2;
        auto [iFirstId, iLastId] = m_vectorFreshIdRanges[iMiddleRangeIndex];
        if (iIngredientId < iFirstId) {
            iLastRangeIndex = iMiddleRangeIndex - 1;
        } else if (iIngredientId > iLastId) {
            iFirstRangeIndex = iMiddleRangeIndex + 1;
        } else {
            return true;
        }
    }
    return false;
}

// ------------------------------------------------------------
// Part 1
// ------------------------------------------------------------

std::string Day05::Part1() {
    int iFreshIngredientCount = 0;
    for (auto iIngredientId : m_vectorAvailableIngredientIds) {
        if (IsFresh(iIngredientId))
            ++iFreshIngredientCount;
    }
    return std::to_string(iFreshIngredientCount);
}

// ------------------------------------------------------------
// Part 2
// ------------------------------------------------------------

std::string Day05::Part2() {
    std::int64_t iFreshIdCount = 0;
    for (auto [iFirstId, iLastId] : m_vectorFreshIdRanges) {
        const auto iIdDifference = iLastId - iFirstId;
        if (iIdDifference >= std::numeric_limits<std::int64_t>::max() - iFreshIdCount)
            throw std::overflow_error("Fresh ID count exceeds int64_t");
        iFreshIdCount += iIdDifference + 1;
    }
    return std::to_string(iFreshIdCount);
}
