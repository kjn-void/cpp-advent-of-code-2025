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
        rgmov_.clear();
        rgmov_.reserve(rgusLines.size());
        for (const auto& usLine : rgusLines) {
            const auto usRotation = core::UsTrim(usLine);
            if (usRotation.empty())
                continue;
            if (usRotation.front() != 'L' && usRotation.front() != 'R') {
                throw std::invalid_argument("Rotation must start with L or R");
            }
            const auto dposRotation = core::ValParseInteger<std::int64_t>(usRotation.substr(1));
            if (dposRotation < 0)
                throw std::invalid_argument("Rotation distance must be nonnegative");
            rgmov_.push_back({usRotation.front() == 'L', dposRotation});
        }
    }

    std::string TxtPart1() override { return TxtSolve(false); }
    std::string TxtPart2() override { return TxtSolve(true); }

  private:
    struct Mov {
        bool fLeft;
        std::int64_t dposRotation;
    };
    std::vector<Mov> rgmov_;

    std::string TxtSolve(bool fCountCrossings) const {
        int posDial = 50;
        std::int64_t cntZeros = 0;
        for (const auto& [fLeft, dposRotation] : rgmov_) {
            const auto dposRemainder = static_cast<int>(dposRotation % 100);
            if (fCountCrossings) {
                cntZeros += dposRotation / 100;
                // Starting on zero does not itself count as a crossing.
                const int dposToZero = fLeft ? (posDial == 0 ? 100 : posDial) : 100 - posDial;
                cntZeros += dposRemainder >= dposToZero;
            }
            posDial = (posDial + (fLeft ? -dposRemainder : dposRemainder) + 100) % 100;
            if (!fCountCrossings && posDial == 0)
                ++cntZeros;
        }
        return std::to_string(cntZeros);
    }
};

namespace {
const core::Drg<Day01> drgDay{1};
} // namespace
