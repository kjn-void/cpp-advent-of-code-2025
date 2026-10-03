#include "days/day06.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <stdexcept>

// -----------------------------------------------------------------------------
// Registration
// -----------------------------------------------------------------------------
namespace {
const core::DayRegistration<Day06> registration{6};
} // namespace

// -----------------------------------------------------------------------------
// Input
// -----------------------------------------------------------------------------

void Day06::set_input(const std::vector<std::string>& lines) {
    grid_ = lines;

    // normalize width
    C_ = 0;
    for (const auto& row : grid_) {
        C_ = std::max(C_, static_cast<int>(row.size()));
    }
    for (auto& row : grid_) {
        if (static_cast<int>(row.size()) < C_) {
            row.append(C_ - row.size(), ' ');
        }
    }

    R_ = static_cast<int>(grid_.size());
}

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

std::vector<Day06::Block> Day06::find_blocks() const {
    std::vector<bool> blank(C_, true);

    for (int c = 0; c < C_; ++c) {
        for (int r = 0; r < R_; ++r) {
            if (grid_[r][c] != ' ') {
                blank[c] = false;
                break;
            }
        }
    }

    std::vector<Block> blocks;
    bool in_block = false;
    int start = 0;

    for (int c = 0; c < C_; ++c) {
        if (!blank[c]) {
            if (!in_block) {
                in_block = true;
                start = c;
            }
        } else if (in_block) {
            blocks.push_back({start, c - 1});
            in_block = false;
        }
    }

    if (in_block) {
        blocks.push_back({start, C_ - 1});
    }

    return blocks;
}

char Day06::get_operator(const Block& b) const {
    const auto& row = grid_[R_ - 1];
    for (int c = b.start; c <= b.end; ++c) {
        if (row[c] == '+' || row[c] == '*') {
            return row[c];
        }
    }
    throw std::invalid_argument("Missing worksheet operator");
}

// -----------------------------------------------------------------------------
// Extractors
// -----------------------------------------------------------------------------

std::vector<std::int64_t> Day06::extract_part1(const Block& b) const {
    std::vector<std::int64_t> nums;
    nums.reserve(R_);

    for (int r = 0; r < R_ - 1; ++r) {
        std::string s = grid_[r].substr(b.start, b.end - b.start + 1);
        s.erase(0, s.find_first_not_of(' '));
        s.erase(s.find_last_not_of(' ') + 1);
        nums.push_back(core::parse_integer<std::int64_t>(s));
    }
    return nums;
}

std::vector<std::int64_t> Day06::extract_part2(const Block& b) const {
    std::vector<std::int64_t> nums;
    nums.reserve(b.end - b.start + 1);

    for (int c = b.start; c <= b.end; ++c) {
        std::string s;
        for (int r = 0; r < R_ - 1; ++r) {
            char ch = grid_[r][c];
            if (ch != ' ')
                s.push_back(ch);
        }
        nums.push_back(core::parse_integer<std::int64_t>(s));
    }
    return nums;
}

// -----------------------------------------------------------------------------
// Evaluation
// -----------------------------------------------------------------------------

std::int64_t Day06::eval_numbers(std::span<const std::int64_t> nums, char op) {
    if (op == '+') {
        std::int64_t sum = 0;
        for (auto v : nums)
            sum += v;
        return sum;
    }

    std::int64_t prod = 1;
    for (auto v : nums)
        prod *= v;
    return prod;
}

template <typename Extractor> std::int64_t Day06::evaluate_blocks(Extractor&& extractor) const {
    std::int64_t total = 0;

    for (const auto& b : find_blocks()) {
        auto nums = extractor(b);
        char op = get_operator(b);
        total += eval_numbers(nums, op);
    }
    return total;
}

// -----------------------------------------------------------------------------
// Parts
// -----------------------------------------------------------------------------

std::string Day06::part1() {
    return std::to_string(evaluate_blocks([this](const Block& b) { return extract_part1(b); }));
}

std::string Day06::part2() {
    return std::to_string(evaluate_blocks([this](const Block& b) { return extract_part2(b); }));
}
