#include "days/day05.h"
#include "core/Registry.h"

#include <algorithm>
#include <sstream>

// ------------------------------------------------------------
// Registration
// ------------------------------------------------------------
static bool registered_day05 = [] {
    Registry::instance().register_day(
        5, [] { return std::make_unique<Day05>(); }
    );
    return true;
}();

// ------------------------------------------------------------

void Day05::set_input(const std::vector<std::string>& lines) {
    ranges_.clear();
    ids_.clear();

    int section = 0;

    for (const auto& line : lines) {
        if (line.empty()) {
            ++section;
            continue;
        }

        if (section == 0) {
            // range
            auto dash = line.find('-');
            int64_t lo = std::stoll(line.substr(0, dash));
            int64_t hi = std::stoll(line.substr(dash + 1));
            ranges_.emplace_back(lo, hi);
        } else {
            // id
            ids_.push_back(std::stoll(line));
        }
    }

    // merge overlapping ranges
    std::sort(ranges_.begin(), ranges_.end());

    std::vector<std::pair<int64_t, int64_t>> merged;
    int64_t cur_lo = ranges_[0].first;
    int64_t cur_hi = ranges_[0].second;

    for (size_t i = 1; i < ranges_.size(); ++i) {
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

bool Day05::is_fresh(int64_t id) const {
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
    int64_t total = 0;
    for (auto [lo, hi] : ranges_) {
        total += (hi - lo + 1);
    }
    return std::to_string(total);
}