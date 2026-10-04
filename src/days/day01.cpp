#include "core/Parse.h"
#include "core/Register.h"
#include "core/Solution.h"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

class Day01 final : public Solution {
  public:
    void SetInput(const std::vector<std::string>& vectorInputLines) override {
        m_vectorRotations.clear();
        m_vectorRotations.reserve(vectorInputLines.size());
        for (const auto& stringLine : vectorInputLines) {
            const auto stringRotation = core::Trim(stringLine);
            if (stringRotation.empty())
                continue;
            if (stringRotation.front() != 'L' && stringRotation.front() != 'R') {
                throw std::invalid_argument("Rotation must start with L or R");
            }
            const auto iClickCount = core::ParseInteger<std::int64_t>(stringRotation.substr(1));
            if (iClickCount < 0)
                throw std::invalid_argument("Rotation distance must be nonnegative");
            m_vectorRotations.push_back({stringRotation.front() == 'L', iClickCount});
        }
    }

    std::string Part1() override { return CalculatePassword(false); }
    std::string Part2() override { return CalculatePassword(true); }

  private:
    struct Rotation {
        bool m_bTurnsLeft;
        std::int64_t m_iClickCount;
    };
    std::vector<Rotation> m_vectorRotations;

    std::string CalculatePassword(bool bCountZeroClicks) const {
        int iDialPosition = 50;
        std::int64_t iZeroCount = 0;
        for (const auto& [bTurnsLeft, iClickCount] : m_vectorRotations) {
            const auto iRemainingClicks = static_cast<int>(iClickCount % 100);
            if (bCountZeroClicks) {
                iZeroCount += iClickCount / 100;
                // Starting on zero does not itself count as a click onto zero.
                const int iClicksToZero =
                    bTurnsLeft ? (iDialPosition == 0 ? 100 : iDialPosition) : 100 - iDialPosition;
                iZeroCount += iRemainingClicks >= iClicksToZero;
            }
            iDialPosition =
                (iDialPosition + (bTurnsLeft ? -iRemainingClicks : iRemainingClicks) + 100) % 100;
            if (!bCountZeroClicks && iDialPosition == 0)
                ++iZeroCount;
        }
        return std::to_string(iZeroCount);
    }
};

namespace {
const core::DayRegistration<Day01> dayregistration{1};
} // namespace
