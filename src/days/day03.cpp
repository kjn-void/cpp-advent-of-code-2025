#include "days/day03.h"
#include "core/Register.h"

#include <cstdint>
#include <stdexcept>
#include <string>

// ------------------------------------------------------------
// Registration
// ------------------------------------------------------------

namespace {
const core::Drg<Day03> drgDay{3};
} // namespace

// ------------------------------------------------------------
// Input
// ------------------------------------------------------------

void Day03::SetInput(const std::vector<std::string>& rgusLines) {
    rgbnk_.clear();
    rgbnk_.reserve(rgusLines.size());

    for (const auto& usLine : rgusLines) {
        std::vector<int> rgdig;
        rgdig.reserve(usLine.size());

        for (char chDigit : usLine) {
            if (chDigit < '0' || chDigit > '9')
                throw std::invalid_argument("Battery bank must contain digits");
            rgdig.push_back(chDigit - '0');
        }

        rgbnk_.push_back(std::move(rgdig));
    }
}

// ------------------------------------------------------------
// Part 1 / Part 2
// ------------------------------------------------------------

std::string Day03::TxtPart1() {
    return TxtMaxJoltage(2);
}

std::string Day03::TxtPart2() {
    return TxtMaxJoltage(12);
}

// ------------------------------------------------------------
// Core logic
// ------------------------------------------------------------

std::string Day03::TxtMaxJoltage(int cdigToSelect) const {
    std::int64_t jolSum = 0;

    for (const auto& bnk : rgbnk_) {
        const int cdig = static_cast<int>(bnk.size());

        int cdigNeeded = cdigToSelect;
        std::vector<int> rgdigSelected;
        rgdigSelected.reserve(cdigToSelect);

        for (int idigBattery = 0; idigBattery < cdig; ++idigBattery) {
            int dig = bnk[idigBattery];

            int cdigRemaining = cdig - idigBattery;
            bool fCanDiscard = !rgdigSelected.empty() && cdigRemaining > cdigNeeded;

            while (fCanDiscard && rgdigSelected.back() < dig) {
                rgdigSelected.pop_back();
                ++cdigNeeded;
                fCanDiscard = !rgdigSelected.empty() && cdigRemaining > cdigNeeded;
            }

            if (cdigNeeded > 0) {
                rgdigSelected.push_back(dig);
                --cdigNeeded;
            }
        }

        jolSum += JolFromDigits(rgdigSelected);
    }

    return std::to_string(jolSum);
}

std::int64_t Day03::JolFromDigits(std::span<const int> rgdigSelected) {
    std::int64_t jolSum = 0;
    for (int dig : rgdigSelected) {
        jolSum = jolSum * 10 + dig;
    }
    return jolSum;
}
