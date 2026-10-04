#include "days/day03.h"
#include "core/Register.h"

#include <cstdint>
#include <stdexcept>
#include <string>

// ------------------------------------------------------------
// Registration
// ------------------------------------------------------------

namespace {
const core::DayRegistration<Day03> registration{3};
} // namespace

// ------------------------------------------------------------
// Input
// ------------------------------------------------------------

void Day03::set_input(const std::vector<std::string>& input_lines) {
    battery_banks_.clear();
    battery_banks_.reserve(input_lines.size());

    for (const auto& line : input_lines) {
        std::vector<int> battery_ratings;
        battery_ratings.reserve(line.size());

        for (char digit : line) {
            if (digit < '0' || digit > '9')
                throw std::invalid_argument("Battery bank must contain digits");
            battery_ratings.push_back(digit - '0');
        }

        battery_banks_.push_back(std::move(battery_ratings));
    }
}

// ------------------------------------------------------------
// Part 1 / Part 2
// ------------------------------------------------------------

std::string Day03::part1() {
    return total_output_joltage(2);
}

std::string Day03::part2() {
    return total_output_joltage(12);
}

// ------------------------------------------------------------
// Core logic
// ------------------------------------------------------------

std::string Day03::total_output_joltage(int batteries_to_select) const {
    std::int64_t total_joltage = 0;

    for (const auto& bank : battery_banks_) {
        const int battery_count = static_cast<int>(bank.size());

        int batteries_needed = batteries_to_select;
        std::vector<int> selected_ratings;
        selected_ratings.reserve(batteries_to_select);

        for (int battery_index = 0; battery_index < battery_count; ++battery_index) {
            int rating = bank[battery_index];

            int batteries_remaining = battery_count - battery_index;
            bool can_discard = !selected_ratings.empty() && batteries_remaining > batteries_needed;

            while (can_discard && selected_ratings.back() < rating) {
                selected_ratings.pop_back();
                ++batteries_needed;
                can_discard = !selected_ratings.empty() && batteries_remaining > batteries_needed;
            }

            if (batteries_needed > 0) {
                selected_ratings.push_back(rating);
                --batteries_needed;
            }
        }

        total_joltage += joltage_from_ratings(selected_ratings);
    }

    return std::to_string(total_joltage);
}

std::int64_t Day03::joltage_from_ratings(std::span<const int> selected_ratings) {
    std::int64_t bank_joltage = 0;
    for (int rating : selected_ratings) {
        bank_joltage = bank_joltage * 10 + rating;
    }
    return bank_joltage;
}
