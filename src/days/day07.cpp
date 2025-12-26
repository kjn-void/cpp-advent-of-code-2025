#include "days/day07.h"
#include "core/Registry.h"

#include <algorithm>
#include <numeric>

// 🔌 Static registration
static bool registered_day07 = [] {
    Registry::instance().register_day(
        7,
        [] { return std::make_unique<Day07>(); }
    );
    return true;
}();

// ------------------------------------------------------------

void Day07::set_input(const std::vector<std::string>& lines) {
    grid_.clear();
    grid_ = lines;

    // Normalize width
    cols_ = static_cast<int>(grid_[0].size());
    for (auto& row : grid_) {
        if (static_cast<int>(row.size()) < cols_) {
            row.append(cols_ - row.size(), ' ');
        }
    }

    rows_ = static_cast<int>(grid_.size());

    // Find S in first row
    for (int c = 0; c < cols_; ++c) {
        if (grid_[0][c] == 'S') {
            start_col_ = c;
            break;
        }
    }
}

// ------------------------------------------------------------
// Part 1 — count splits
// ------------------------------------------------------------

std::string Day07::part1() {
    std::vector<bool> bufA(cols_, false);
    std::vector<bool> bufB(cols_, false);

    auto* active = &bufA;
    auto* next   = &bufB;

    (*active)[start_col_] = true;

    int split_count = 0;

    for (int r = 1; r < rows_; ++r) {
        std::fill(next->begin(), next->end(), false);
        const auto& row = grid_[r];

        for (int c = 0; c < cols_; ++c) {
            if (!(*active)[c]) continue;

            if (row[c] == '^') {
                split_count++;
                if (c > 0)         (*next)[c - 1] = true;
                if (c + 1 < cols_) (*next)[c + 1] = true;
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
    std::vector<int64_t> bufA(cols_, 0);
    std::vector<int64_t> bufB(cols_, 0);

    auto* active = &bufA;
    auto* next   = &bufB;

    (*active)[start_col_] = 1;

    for (int r = 1; r < rows_; ++r) {
        std::fill(next->begin(), next->end(), 0);
        const auto& row = grid_[r];

        for (int c = 0; c < cols_; ++c) {
            int64_t count = (*active)[c];
            if (count == 0) continue;

            if (row[c] == '^') {
                if (c > 0)         (*next)[c - 1] += count;
                if (c + 1 < cols_) (*next)[c + 1] += count;
            } else {
                (*next)[c] += count;
            }
        }

        std::swap(active, next);
    }

    int64_t total =
        std::accumulate(active->begin(), active->end(), int64_t{0});

    return std::to_string(total);
}