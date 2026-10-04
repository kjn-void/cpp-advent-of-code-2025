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

void Day07::set_input(const std::vector<std::string>& input_lines) {
    manifold_ = input_lines;
    row_count_ = static_cast<int>(manifold_.size());
    column_count_ = 0;
    start_column_ = -1;
    for (const auto& input_row : manifold_)
        column_count_ = std::max(column_count_, static_cast<int>(input_row.size()));
    if (column_count_ == 0)
        return;
    for (auto& input_row : manifold_)
        input_row.resize(column_count_, '.');
    const auto start_column = manifold_.front().find('S');
    if (start_column == std::string::npos ||
        manifold_.front().find('S', start_column + 1) != std::string::npos)
        throw std::invalid_argument("Tachyon grid must have one start in its first row");
    start_column_ = static_cast<int>(start_column);
}

// ------------------------------------------------------------
// Part 1 — count splits
// ------------------------------------------------------------

std::string Day07::part1() {
    if (start_column_ < 0)
        return "0";
    std::vector<bool> beam_buffer_a(column_count_, false);
    std::vector<bool> beam_buffer_b(column_count_, false);

    auto* active_beams = &beam_buffer_a;
    auto* next_beams = &beam_buffer_b;

    (*active_beams)[start_column_] = true;

    int split_count = 0;

    for (int row = 1; row < row_count_; ++row) {
        std::fill(next_beams->begin(), next_beams->end(), false);
        const auto& input_row = manifold_[row];

        for (int column = 0; column < column_count_; ++column) {
            if (!(*active_beams)[column])
                continue;

            if (input_row[column] == '^') {
                split_count++;
                if (column > 0)
                    (*next_beams)[column - 1] = true;
                if (column + 1 < column_count_)
                    (*next_beams)[column + 1] = true;
            } else {
                (*next_beams)[column] = true;
            }
        }

        std::swap(active_beams, next_beams);
    }

    return std::to_string(split_count);
}

// ------------------------------------------------------------
// Part 2 — count timelines
// ------------------------------------------------------------

std::string Day07::part2() {
    if (start_column_ < 0)
        return "0";
    std::int64_t exited_timelines = 0;
    std::vector<std::int64_t> timeline_buffer_a(column_count_, 0);
    std::vector<std::int64_t> timeline_buffer_b(column_count_, 0);

    auto* active_timelines = &timeline_buffer_a;
    auto* next_timelines = &timeline_buffer_b;

    (*active_timelines)[start_column_] = 1;

    for (int row = 1; row < row_count_; ++row) {
        std::fill(next_timelines->begin(), next_timelines->end(), 0);
        const auto& input_row = manifold_[row];

        for (int column = 0; column < column_count_; ++column) {
            std::int64_t timeline_count = (*active_timelines)[column];
            if (timeline_count == 0)
                continue;

            if (input_row[column] == '^') {
                if (column > 0)
                    (*next_timelines)[column - 1] += timeline_count;
                else
                    exited_timelines += timeline_count;
                if (column + 1 < column_count_)
                    (*next_timelines)[column + 1] += timeline_count;
                else
                    exited_timelines += timeline_count;
            } else {
                (*next_timelines)[column] += timeline_count;
            }
        }

        std::swap(active_timelines, next_timelines);
    }

    std::int64_t total_timelines =
        std::accumulate(active_timelines->begin(), active_timelines->end(), std::int64_t{0});

    return std::to_string(total_timelines + exited_timelines);
}
