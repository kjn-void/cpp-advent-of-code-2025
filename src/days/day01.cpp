#include "core/Parse.h"
#include "core/Register.h"
#include "core/Solution.h"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

class Day01 final : public Slv {
  public:
    void SetInput(const std::vector<std::string>& rgusLines) override {
        rgrot_.clear();
        rgrot_.reserve(rgusLines.size());
        for (const auto& usLine : rgusLines) {
            const auto usRotation = core::UsTrim(usLine);
            if (usRotation.empty())
                continue;
            if (usRotation.front() != 'L' && usRotation.front() != 'R') {
                throw std::invalid_argument("Rotation must start with L or R");
            }
            const auto cclkRotation = core::ValParseInteger<std::int64_t>(usRotation.substr(1));
            if (cclkRotation < 0)
                throw std::invalid_argument("Rotation distance must be nonnegative");
            rgrot_.push_back({usRotation.front() == 'L', cclkRotation});
        }
    }

    std::string TxtPart1() override { return TxtPassword(false); }
    std::string TxtPart2() override { return TxtPassword(true); }

  private:
    struct Rot {
        bool fLeft;
        std::int64_t cclkRotation;
    };
    std::vector<Rot> rgrot_;

    std::string TxtPassword(bool fCountZeroClicks) const {
        int posDial = 50;
        std::int64_t cntZeroVisits = 0;
        for (const auto& [fLeft, cclkRotation] : rgrot_) {
            const auto cclkRemainder = static_cast<int>(cclkRotation % 100);
            if (fCountZeroClicks) {
                cntZeroVisits += cclkRotation / 100;
                // Starting on zero does not itself count as a crossing.
                const int cclkToZero = fLeft ? (posDial == 0 ? 100 : posDial) : 100 - posDial;
                cntZeroVisits += cclkRemainder >= cclkToZero;
            }
            posDial = (posDial + (fLeft ? -cclkRemainder : cclkRemainder) + 100) % 100;
            if (!fCountZeroClicks && posDial == 0)
                ++cntZeroVisits;
        }
        return std::to_string(cntZeroVisits);
    }
};

namespace {
const core::Drg<Day01> drgDay{1};
} // namespace
