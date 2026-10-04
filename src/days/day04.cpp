#include "days/day04.h"
#include "core/Register.h"

#include <algorithm>
#include <array>
#include <queue>
#include <stdexcept>

// ------------------------------------------------------------
// Registration (static init)
// ------------------------------------------------------------
namespace {
const core::DayRegistration<Day04> registration{4};
} // namespace

// ------------------------------------------------------------
// Direction table (8 neighbors)
// ------------------------------------------------------------
static constexpr std::array<std::pair<int, int>, 8> neighbor_offsets{
    {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

// ------------------------------------------------------------

void Day04::set_input(const std::vector<std::string>& input_lines) {
    if (!input_lines.empty() && !std::ranges::all_of(input_lines, [&](const auto& input_row) {
            return input_row.size() == input_lines.front().size();
        }))
        throw std::invalid_argument("Paper roll grid must be rectangular");
    paper_roll_diagram_ = input_lines;
    row_count_ = static_cast<int>(paper_roll_diagram_.size());
    column_count_ = row_count_ ? static_cast<int>(paper_roll_diagram_[0].size()) : 0;
}

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

int Day04::count_adjacent_rolls(int row, int column) const {
    int adjacent_roll_count = 0;
    for (auto [row_offset, column_offset] : neighbor_offsets) {
        int neighbor_row = row + row_offset;
        int neighbor_column = column + column_offset;
        if (neighbor_row >= 0 && neighbor_row < row_count_ && neighbor_column >= 0 &&
            neighbor_column < column_count_ &&
            paper_roll_diagram_[neighbor_row][neighbor_column] == '@') {
            ++adjacent_roll_count;
        }
    }
    return adjacent_roll_count;
}

// ------------------------------------------------------------
// Part 1
// ------------------------------------------------------------

std::string Day04::part1() {
    if (row_count_ == 0 || column_count_ == 0)
        return "0";

    int accessible_roll_count = 0;
    for (int row = 0; row < row_count_; ++row) {
        for (int column = 0; column < column_count_; ++column) {
            if (paper_roll_diagram_[row][column] != '@')
                continue;
            if (count_adjacent_rolls(row, column) < 4)
                ++accessible_roll_count;
        }
    }
    return std::to_string(accessible_roll_count);
}

// ------------------------------------------------------------
// Part 2
// ------------------------------------------------------------

std::string Day04::part2() {
    if (row_count_ == 0 || column_count_ == 0)
        return "0";

    // Rolls still in the diagram
    std::vector<std::vector<bool>> has_roll(row_count_, std::vector<bool>(column_count_, false));
    for (int row = 0; row < row_count_; ++row)
        for (int column = 0; column < column_count_; ++column)
            has_roll[row][column] = (paper_roll_diagram_[row][column] == '@');

    // Adjacent roll count for each roll
    std::vector<std::vector<int>> adjacent_roll_counts(row_count_,
                                                       std::vector<int>(column_count_, 0));
    for (int row = 0; row < row_count_; ++row) {
        for (int column = 0; column < column_count_; ++column) {
            if (!has_roll[row][column])
                continue;
            for (auto [row_offset, column_offset] : neighbor_offsets) {
                int neighbor_row = row + row_offset;
                int neighbor_column = column + column_offset;
                if (neighbor_row >= 0 && neighbor_row < row_count_ && neighbor_column >= 0 &&
                    neighbor_column < column_count_ && has_roll[neighbor_row][neighbor_column]) {
                    ++adjacent_roll_counts[row][column];
                }
            }
        }
    }

    struct Cell {
        int row, column;
    };
    std::queue<Cell> removable_rolls;

    for (int row = 0; row < row_count_; ++row)
        for (int column = 0; column < column_count_; ++column)
            if (has_roll[row][column] && adjacent_roll_counts[row][column] < 4)
                removable_rolls.push({row, column});

    int removed_roll_count = 0;

    while (!removable_rolls.empty()) {
        auto [row, column] = removable_rolls.front();
        removable_rolls.pop();

        if (!has_roll[row][column])
            continue;

        has_roll[row][column] = false;
        ++removed_roll_count;

        for (auto [row_offset, column_offset] : neighbor_offsets) {
            int neighbor_row = row + row_offset;
            int neighbor_column = column + column_offset;
            if (neighbor_row < 0 || neighbor_row >= row_count_ || neighbor_column < 0 ||
                neighbor_column >= column_count_)
                continue;
            if (!has_roll[neighbor_row][neighbor_column])
                continue;

            if (--adjacent_roll_counts[neighbor_row][neighbor_column] == 3)
                removable_rolls.push({neighbor_row, neighbor_column});
        }
    }

    return std::to_string(removed_roll_count);
}
