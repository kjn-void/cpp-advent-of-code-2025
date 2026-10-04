#include "days/day05.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <limits>
#include <stdexcept>

// ------------------------------------------------------------
// Registration
// ------------------------------------------------------------
namespace {
const core::DayRegistration<Day05> registration{5};
} // namespace

// ------------------------------------------------------------

void Day05::set_input(const std::vector<std::string>& input_lines) {
    fresh_id_ranges_.clear();
    available_ingredient_ids_.clear();

    int section_index = 0;

    for (const auto& raw_line : input_lines) {
        const auto line = core::trim(raw_line);
        if (line.empty()) {
            ++section_index;
            continue;
        }

        if (section_index == 0) {
            // range
            auto dash_offset = line.find('-');
            if (dash_offset == std::string_view::npos)
                throw std::invalid_argument("Expected a fresh ID range");
            const auto first_id = core::parse_integer<std::int64_t>(line.substr(0, dash_offset));
            const auto last_id = core::parse_integer<std::int64_t>(line.substr(dash_offset + 1));
            if (first_id < 0 || last_id < first_id)
                throw std::invalid_argument("Invalid fresh ID range");
            fresh_id_ranges_.emplace_back(first_id, last_id);
        } else {
            // id
            available_ingredient_ids_.push_back(core::parse_integer<std::int64_t>(line));
        }
    }

    // merge overlapping ranges
    std::ranges::sort(fresh_id_ranges_);
    if (fresh_id_ranges_.empty())
        return;

    std::vector<std::pair<std::int64_t, std::int64_t>> merged_ranges;
    std::int64_t merged_first_id = fresh_id_ranges_[0].first;
    std::int64_t merged_last_id = fresh_id_ranges_[0].second;

    for (std::size_t range_index = 1; range_index < fresh_id_ranges_.size(); ++range_index) {
        auto [first_id, last_id] = fresh_id_ranges_[range_index];
        if (first_id <= merged_last_id) {
            merged_last_id = std::max(merged_last_id, last_id);
        } else {
            merged_ranges.emplace_back(merged_first_id, merged_last_id);
            merged_first_id = first_id;
            merged_last_id = last_id;
        }
    }
    merged_ranges.emplace_back(merged_first_id, merged_last_id);
    fresh_id_ranges_.swap(merged_ranges);
}

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

bool Day05::is_fresh(std::int64_t ingredient_id) const {
    // binary search in merged ranges
    int first_range_index = 0;
    int last_range_index = static_cast<int>(fresh_id_ranges_.size()) - 1;

    while (first_range_index <= last_range_index) {
        int middle_range_index = (first_range_index + last_range_index) / 2;
        auto [first_id, last_id] = fresh_id_ranges_[middle_range_index];
        if (ingredient_id < first_id) {
            last_range_index = middle_range_index - 1;
        } else if (ingredient_id > last_id) {
            first_range_index = middle_range_index + 1;
        } else {
            return true;
        }
    }
    return false;
}

// ------------------------------------------------------------
// Part 1
// ------------------------------------------------------------

std::string Day05::part1() {
    int fresh_ingredient_count = 0;
    for (auto ingredient_id : available_ingredient_ids_) {
        if (is_fresh(ingredient_id))
            ++fresh_ingredient_count;
    }
    return std::to_string(fresh_ingredient_count);
}

// ------------------------------------------------------------
// Part 2
// ------------------------------------------------------------

std::string Day05::part2() {
    std::int64_t fresh_id_count = 0;
    for (auto [first_id, last_id] : fresh_id_ranges_) {
        const auto id_difference = last_id - first_id;
        if (id_difference >= std::numeric_limits<std::int64_t>::max() - fresh_id_count)
            throw std::overflow_error("Fresh ID count exceeds int64_t");
        fresh_id_count += id_difference + 1;
    }
    return std::to_string(fresh_id_count);
}
