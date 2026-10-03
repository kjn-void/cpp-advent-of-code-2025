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
    void set_input(const std::vector<std::string>& lines) override {
        moves_.clear();
        moves_.reserve(lines.size());
        for (const auto& line : lines) {
            const auto text = core::trim(line);
            if (text.empty())
                continue;
            if (text.front() != 'L' && text.front() != 'R') {
                throw std::invalid_argument("Rotation must start with L or R");
            }
            const auto distance = core::parse_integer<std::int64_t>(text.substr(1));
            if (distance < 0)
                throw std::invalid_argument("Rotation distance must be nonnegative");
            moves_.push_back({text.front() == 'L', distance});
        }
    }

    std::string part1() override { return solve(false); }
    std::string part2() override { return solve(true); }

  private:
    struct Move {
        bool left;
        std::int64_t distance;
    };
    std::vector<Move> moves_;

    std::string solve(bool count_crossings) const {
        int position = 50;
        std::int64_t zeros = 0;
        for (const auto& [left, distance] : moves_) {
            const auto remainder = static_cast<int>(distance % 100);
            if (count_crossings) {
                zeros += distance / 100;
                // Starting on zero does not itself count as a crossing.
                const int to_zero = left ? (position == 0 ? 100 : position) : 100 - position;
                zeros += remainder >= to_zero;
            }
            position = (position + (left ? -remainder : remainder) + 100) % 100;
            if (!count_crossings && position == 0)
                ++zeros;
        }
        return std::to_string(zeros);
    }
};

namespace {
const core::DayRegistration<Day01> registration{1};
} // namespace
