#include "days/day03.h"
#include "core/Register.h"

#include <cstdint>
#include <stdexcept>
#include <string>

// ------------------------------------------------------------
// Registration
// ------------------------------------------------------------

namespace {
const core::DayRegistration<Day03> dayregistration{3};
} // namespace

// ------------------------------------------------------------
// Input
// ------------------------------------------------------------

void Day03::SetInput(const std::vector<std::string>& vectorInputLines) {
    m_vectorBatteryBanks.clear();
    m_vectorBatteryBanks.reserve(vectorInputLines.size());

    for (const auto& stringLine : vectorInputLines) {
        std::vector<int> vectorBatteryRatings;
        vectorBatteryRatings.reserve(stringLine.size());

        for (char iDigit : stringLine) {
            if (iDigit < '0' || iDigit > '9')
                throw std::invalid_argument("Battery bank must contain digits");
            vectorBatteryRatings.push_back(iDigit - '0');
        }

        m_vectorBatteryBanks.push_back(std::move(vectorBatteryRatings));
    }
}

// ------------------------------------------------------------
// Part 1 / Part 2
// ------------------------------------------------------------

std::string Day03::Part1() {
    return TotalOutputJoltage(2);
}

std::string Day03::Part2() {
    return TotalOutputJoltage(12);
}

// ------------------------------------------------------------
// Core logic
// ------------------------------------------------------------

std::string Day03::TotalOutputJoltage(int iBatteriesToSelect) const {
    std::int64_t iTotalJoltage = 0;

    for (const auto& vectorBank : m_vectorBatteryBanks) {
        const int iBatteryCount = static_cast<int>(vectorBank.size());

        int iBatteriesNeeded = iBatteriesToSelect;
        std::vector<int> vectorSelectedRatings;
        vectorSelectedRatings.reserve(iBatteriesToSelect);

        for (int iBatteryIndex = 0; iBatteryIndex < iBatteryCount; ++iBatteryIndex) {
            int iRating = vectorBank[iBatteryIndex];

            int iBatteriesRemaining = iBatteryCount - iBatteryIndex;
            bool bCanDiscard =
                !vectorSelectedRatings.empty() && iBatteriesRemaining > iBatteriesNeeded;

            while (bCanDiscard && vectorSelectedRatings.back() < iRating) {
                vectorSelectedRatings.pop_back();
                ++iBatteriesNeeded;
                bCanDiscard =
                    !vectorSelectedRatings.empty() && iBatteriesRemaining > iBatteriesNeeded;
            }

            if (iBatteriesNeeded > 0) {
                vectorSelectedRatings.push_back(iRating);
                --iBatteriesNeeded;
            }
        }

        iTotalJoltage += JoltageFromRatings(vectorSelectedRatings);
    }

    return std::to_string(iTotalJoltage);
}

std::int64_t Day03::JoltageFromRatings(std::span<const int> spanSelectedRatings) {
    std::int64_t iBankJoltage = 0;
    for (int iRating : spanSelectedRatings) {
        iBankJoltage = iBankJoltage * 10 + iRating;
    }
    return iBankJoltage;
}
