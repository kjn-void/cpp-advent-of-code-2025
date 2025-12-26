#include "days/day03.h"
#include "core/Registry.h"

#include <cstdint>
#include <string>

// ------------------------------------------------------------
// Registration (static init, like Go/Rust)
// ------------------------------------------------------------

static bool registered_day03 = [] {
    Registry::instance().register_day(
        3,
        [] { return std::make_unique<Day03>(); }
    );
    return true;
}();

// ------------------------------------------------------------
// Input
// ------------------------------------------------------------

void Day03::set_input(const std::vector<std::string>& lines) {
    banks.clear();
    banks.reserve(lines.size());

    for (const auto& line : lines) {
        std::vector<int> digits;
        digits.reserve(line.size());

        for (char ch : line) {
            digits.push_back(ch - '0'); // 1–9
        }

        banks.push_back(std::move(digits));
    }
}

// ------------------------------------------------------------
// Part 1 / Part 2
// ------------------------------------------------------------

std::string Day03::part1() {
    return max_joltage(2);
}

std::string Day03::part2() {
    return max_joltage(12);
}

// ------------------------------------------------------------
// Core logic
// ------------------------------------------------------------

std::string Day03::max_joltage(int pick) const {
    int64_t total = 0;

    for (const auto& bank : banks) {
        const int n = static_cast<int>(bank.size());

        int need = pick;
        std::vector<int> stack;
        stack.reserve(pick);

        for (int i = 0; i < n; ++i) {
            int dig = bank[i];

            int remaining = n - i;
            bool can_pop = !stack.empty() && remaining > need;

            while (can_pop && stack.back() < dig) {
                stack.pop_back();
                ++need;
                can_pop = !stack.empty() && remaining > need;
            }

            if (need > 0) {
                stack.push_back(dig);
                --need;
            }
        }

        total += stack_to_number(stack);
    }

    return std::to_string(total);
}

int64_t Day03::stack_to_number(const std::vector<int>& stack) {
    int64_t value = 0;
    for (int d : stack) {
        value = value * 10 + d;
    }
    return value;
}