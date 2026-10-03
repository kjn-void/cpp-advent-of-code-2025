#include "days/day07.h"
#include "core/Register.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>

// Registration
namespace {
const core::DayRegistration<Day07> registration{7};
} // namespace

// ------------------------------------------------------------

void Day07::set_input(const std::vector<std::string>& lines) {
    grid_ = lines;
    rows_ = static_cast<int>(grid_.size());
    cols_ = 0;
    start_col_ = -1;
    for (const auto& row : grid_)
        cols_ = std::max(cols_, static_cast<int>(row.size()));
    if (cols_ == 0)
        return;
    for (auto& row : grid_)
        row.resize(cols_, '.');
    const auto start = grid_.front().find('S');
    if (start == std::string::npos || grid_.front().find('S', start + 1) != std::string::npos)
        throw std::invalid_argument("Tachyon grid must have one start in its first row");
    start_col_ = static_cast<int>(start);
}

// ------------------------------------------------------------
// Part 1 — count splits
// ------------------------------------------------------------

std::string Day07::part1() {
    if (start_col_ < 0)
        return "0";
    std::vector<bool> bufA(cols_, false);
    std::vector<bool> bufB(cols_, false);

    auto* active = &bufA;
    auto* next = &bufB;

    (*active)[start_col_] = true;

    int split_count = 0;

    for (int r = 1; r < rows_; ++r) {
        std::fill(next->begin(), next->end(), false);
        const auto& row = grid_[r];

        for (int c = 0; c < cols_; ++c) {
            if (!(*active)[c])
                continue;

            if (row[c] == '^') {
                split_count++;
                if (c > 0)
                    (*next)[c - 1] = true;
                if (c + 1 < cols_)
                    (*next)[c + 1] = true;
            } else {
                (*next)[c] = true;
            }
        }

        std::swap(active, next);
    }

    return std::to_string(split_count);
}

// ------------------------------------------------------------
// Part 2 — count timelines
// ------------------------------------------------------------

std::string Day07::part2() {
    if (start_col_ < 0)
        return "0";
    std::int64_t exited = 0;
    std::vector<std::int64_t> bufA(cols_, 0);
    std::vector<std::int64_t> bufB(cols_, 0);

    auto* active = &bufA;
    auto* next = &bufB;

    (*active)[start_col_] = 1;

    for (int r = 1; r < rows_; ++r) {
        std::fill(next->begin(), next->end(), 0);
        const auto& row = grid_[r];

        for (int c = 0; c < cols_; ++c) {
            std::int64_t count = (*active)[c];
            if (count == 0)
                continue;

            if (row[c] == '^') {
                if (c > 0)
                    (*next)[c - 1] += count;
                else
                    exited += count;
                if (c + 1 < cols_)
                    (*next)[c + 1] += count;
                else
                    exited += count;
            } else {
                (*next)[c] += count;
            }
        }

        std::swap(active, next);
    }

    std::int64_t total = std::accumulate(active->begin(), active->end(), std::int64_t{0});

    return std::to_string(total + exited);
}
