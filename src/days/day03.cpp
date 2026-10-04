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
        std::vector<int> rgbat;
        rgbat.reserve(usLine.size());

        for (char chDigit : usLine) {
            if (chDigit < '0' || chDigit > '9')
                throw std::invalid_argument("Battery bank must contain digits");
            rgbat.push_back(chDigit - '0');
        }

        rgbnk_.push_back(std::move(rgbat));
    }
}

// ------------------------------------------------------------
// Part 1 / Part 2
// ------------------------------------------------------------

std::string Day03::TxtPart1() {
    return TxtTotalJoltage(2);
}

std::string Day03::TxtPart2() {
    return TxtTotalJoltage(12);
}

// ------------------------------------------------------------
// Core logic
// ------------------------------------------------------------

std::string Day03::TxtTotalJoltage(int cbatToSelect) const {
    std::int64_t jolSum = 0;

    for (const auto& bnk : rgbnk_) {
        const int cbat = static_cast<int>(bnk.size());

        int cbatNeeded = cbatToSelect;
        std::vector<int> rgbatSelected;
        rgbatSelected.reserve(cbatToSelect);

        for (int ibat = 0; ibat < cbat; ++ibat) {
            int bat = bnk[ibat];

            int cbatRemaining = cbat - ibat;
            bool fCanDiscard = !rgbatSelected.empty() && cbatRemaining > cbatNeeded;

            while (fCanDiscard && rgbatSelected.back() < bat) {
                rgbatSelected.pop_back();
                ++cbatNeeded;
                fCanDiscard = !rgbatSelected.empty() && cbatRemaining > cbatNeeded;
            }

            if (cbatNeeded > 0) {
                rgbatSelected.push_back(bat);
                --cbatNeeded;
            }
        }

        jolSum += JolFromRatings(rgbatSelected);
    }

    return std::to_string(jolSum);
}

std::int64_t Day03::JolFromRatings(std::span<const int> rgbatSelected) {
    std::int64_t jolBank = 0;
    for (int bat : rgbatSelected) {
        jolBank = jolBank * 10 + bat;
    }
    return jolBank;
}
