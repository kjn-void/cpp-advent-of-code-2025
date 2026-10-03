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

void Day05::set_input(const std::vector<std::string>& lines) {
    ranges_.clear();
    ids_.clear();

    int section = 0;

    for (const auto& raw_line : lines) {
        const auto line = core::trim(raw_line);
        if (line.empty()) {
            ++section;
            continue;
        }

        if (section == 0) {
            // range
            auto dash = line.find('-');
            if (dash == std::string_view::npos)
                throw std::invalid_argument("Expected a fresh ID range");
            const auto lo = core::parse_integer<std::int64_t>(line.substr(0, dash));
            const auto hi = core::parse_integer<std::int64_t>(line.substr(dash + 1));
            if (lo < 0 || hi < lo)
                throw std::invalid_argument("Invalid fresh ID range");
            ranges_.emplace_back(lo, hi);
        } else {
            // id
            ids_.push_back(core::parse_integer<std::int64_t>(line));
        }
    }

    // merge overlapping ranges
    std::ranges::sort(ranges_);
    if (ranges_.empty())
        return;

    std::vector<std::pair<std::int64_t, std::int64_t>> merged;
    std::int64_t cur_lo = ranges_[0].first;
    std::int64_t cur_hi = ranges_[0].second;

    for (std::size_t i = 1; i < ranges_.size(); ++i) {
        auto [lo, hi] = ranges_[i];
        if (lo <= cur_hi) {
            cur_hi = std::max(cur_hi, hi);
        } else {
            merged.emplace_back(cur_lo, cur_hi);
            cur_lo = lo;
            cur_hi = hi;
        }
    }
    merged.emplace_back(cur_lo, cur_hi);
    ranges_.swap(merged);
}

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

bool Day05::is_fresh(std::int64_t id) const {
    // binary search in merged ranges
    int lo = 0;
    int hi = static_cast<int>(ranges_.size()) - 1;

    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        auto [a, b] = ranges_[mid];
        if (id < a) {
            hi = mid - 1;
        } else if (id > b) {
            lo = mid + 1;
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
    int count = 0;
    for (auto id : ids_) {
        if (is_fresh(id))
            ++count;
    }
    return std::to_string(count);
}

// ------------------------------------------------------------
// Part 2
// ------------------------------------------------------------

std::string Day05::part2() {
    std::int64_t total = 0;
    for (auto [lo, hi] : ranges_) {
        const auto difference = hi - lo;
        if (difference >= std::numeric_limits<std::int64_t>::max() - total)
            throw std::overflow_error("Fresh ID count exceeds int64_t");
        total += difference + 1;
    }
    return std::to_string(total);
}
