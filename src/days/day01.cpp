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
    void set_input(const std::vector<std::string>& input_lines) override {
        rotations_.clear();
        rotations_.reserve(input_lines.size());
        for (const auto& line : input_lines) {
            const auto rotation_text = core::trim(line);
            if (rotation_text.empty())
                continue;
            if (rotation_text.front() != 'L' && rotation_text.front() != 'R') {
                throw std::invalid_argument("Rotation must start with L or R");
            }
            const auto click_count = core::parse_integer<std::int64_t>(rotation_text.substr(1));
            if (click_count < 0)
                throw std::invalid_argument("Rotation distance must be nonnegative");
            rotations_.push_back({rotation_text.front() == 'L', click_count});
        }
    }

    std::string part1() override { return calculate_password(false); }
    std::string part2() override { return calculate_password(true); }

  private:
    struct Rotation {
        bool turns_left;
        std::int64_t click_count;
    };
    std::vector<Rotation> rotations_;

    std::string calculate_password(bool count_zero_clicks) const {
        int dial_position = 50;
        std::int64_t zero_count = 0;
        for (const auto& [turns_left, click_count] : rotations_) {
            const auto remaining_clicks = static_cast<int>(click_count % 100);
            if (count_zero_clicks) {
                zero_count += click_count / 100;
                // Starting on zero does not itself count as a click onto zero.
                const int clicks_to_zero =
                    turns_left ? (dial_position == 0 ? 100 : dial_position) : 100 - dial_position;
                zero_count += remaining_clicks >= clicks_to_zero;
            }
            dial_position =
                (dial_position + (turns_left ? -remaining_clicks : remaining_clicks) + 100) % 100;
            if (!count_zero_clicks && dial_position == 0)
                ++zero_count;
        }
        return std::to_string(zero_count);
    }
};

namespace {
const core::DayRegistration<Day01> registration{1};
} // namespace
