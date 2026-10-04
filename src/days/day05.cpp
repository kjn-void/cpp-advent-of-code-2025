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
const core::Drg<Day05> drgDay{5};
} // namespace

// ------------------------------------------------------------

void Day05::SetInput(const std::vector<std::string>& rgusLines) {
    rgrngFresh_.clear();
    rgidAvailable_.clear();

    int iterSection = 0;

    for (const auto& usRawLine : rgusLines) {
        const auto usLine = core::UsTrim(usRawLine);
        if (usLine.empty()) {
            ++iterSection;
            continue;
        }

        if (iterSection == 0) {
            // range
            auto offDash = usLine.find('-');
            if (offDash == std::string_view::npos)
                throw std::invalid_argument("Expected a fresh ID range");
            const auto idFirst = core::ValParseInteger<std::int64_t>(usLine.substr(0, offDash));
            const auto idLast = core::ValParseInteger<std::int64_t>(usLine.substr(offDash + 1));
            if (idFirst < 0 || idLast < idFirst)
                throw std::invalid_argument("Invalid fresh ID range");
            rgrngFresh_.emplace_back(idFirst, idLast);
        } else {
            // id
            rgidAvailable_.push_back(core::ValParseInteger<std::int64_t>(usLine));
        }
    }

    // merge overlapping ranges
    std::ranges::sort(rgrngFresh_);
    if (rgrngFresh_.empty())
        return;

    std::vector<std::pair<std::int64_t, std::int64_t>> rgrngMerged;
    std::int64_t idMergedFirst = rgrngFresh_[0].first;
    std::int64_t idMergedLast = rgrngFresh_[0].second;

    for (std::size_t irng = 1; irng < rgrngFresh_.size(); ++irng) {
        auto [idFirst, idLast] = rgrngFresh_[irng];
        if (idFirst <= idMergedLast) {
            idMergedLast = std::max(idMergedLast, idLast);
        } else {
            rgrngMerged.emplace_back(idMergedFirst, idMergedLast);
            idMergedFirst = idFirst;
            idMergedLast = idLast;
        }
    }
    rgrngMerged.emplace_back(idMergedFirst, idMergedLast);
    rgrngFresh_.swap(rgrngMerged);
}

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

bool Day05::FIsFresh(std::int64_t idIngredient) const {
    // binary search in merged ranges
    int irngFirst = 0;
    int irngLast = static_cast<int>(rgrngFresh_.size()) - 1;

    while (irngFirst <= irngLast) {
        int irngMiddle = (irngFirst + irngLast) / 2;
        auto [idFirst, idLast] = rgrngFresh_[irngMiddle];
        if (idIngredient < idFirst) {
            irngLast = irngMiddle - 1;
        } else if (idIngredient > idLast) {
            irngFirst = irngMiddle + 1;
        } else {
            return true;
        }
    }
    return false;
}

// ------------------------------------------------------------
// Part 1
// ------------------------------------------------------------

std::string Day05::TxtPart1() {
    int cidAvailableFresh = 0;
    for (auto idIngredient : rgidAvailable_) {
        if (FIsFresh(idIngredient))
            ++cidAvailableFresh;
    }
    return std::to_string(cidAvailableFresh);
}

// ------------------------------------------------------------
// Part 2
// ------------------------------------------------------------

std::string Day05::TxtPart2() {
    std::int64_t cidFresh = 0;
    for (auto [idFirst, idLast] : rgrngFresh_) {
        const auto didRange = idLast - idFirst;
        if (didRange >= std::numeric_limits<std::int64_t>::max() - cidFresh)
            throw std::overflow_error("Fresh ID count exceeds int64_t");
        cidFresh += didRange + 1;
    }
    return std::to_string(cidFresh);
}
