#include "days/day02.h"
#include "core/Registry.h"

#include <sstream>
#include <charconv>
#include <array>
#include <algorithm>

// ------------------------------------------------------------
// Registration
// ------------------------------------------------------------
static bool registered = [] {
    Registry::instance().register_day(
        2,
        [] { return std::make_unique<Day02>(); }
    );
    return true;
}();

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

constexpr std::array<int64_t, 18> pow10_table() {
    std::array<int64_t, 18> t{};
    int64_t x = 1;
    for (size_t i = 0; i < t.size(); ++i) {
        t[i] = x;
        x *= 10;
    }
    return t;
}

static constexpr auto POW10 = pow10_table();

int Day02::smallest_block(const std::string& s) {
    const int n = static_cast<int>(s.size());
    for (int k = 1; k <= n / 2; ++k) {
        if (n % k != 0) continue;

        const std::string_view block{s.data(), static_cast<size_t>(k)};
        bool ok = true;

        for (int i = k; i < n; i += k) {
            if (std::string_view{s.data() + i, static_cast<size_t>(k)} != block) {
                ok = false;
                break;
            }
        }

        if (ok) return k;
    }
    return n;
}

// ------------------------------------------------------------
// Input
// ------------------------------------------------------------

void Day02::set_input(const std::vector<std::string>& lines) {
    ranges_.clear();
    if (lines.empty()) return;

    const std::string& line = lines[0];
    size_t pos = 0;

    while (pos < line.size()) {
        size_t comma = line.find(',', pos);
        if (comma == std::string::npos) comma = line.size();

        std::string_view part(line.data() + pos, comma - pos);
        size_t dash = part.find('-');

        int64_t lo = 0, hi = 0;
        std::from_chars(part.data(), part.data() + dash, lo);
        std::from_chars(part.data() + dash + 1, part.data() + part.size(), hi);

        ranges_.emplace_back(lo, hi);
        pos = comma + 1;
    }
}

// ------------------------------------------------------------
// Part 1
// ------------------------------------------------------------

std::string Day02::part1() {
    int64_t sum = 0;

    for (auto [L, R] : ranges_) {
        int max_digits = static_cast<int>(std::to_string(R).size());

        for (int k = 1; 2 * k <= max_digits; ++k) {
            int64_t base = POW10[k];
            int64_t rep = base + 1;

            int64_t d_lo = POW10[k - 1];
            int64_t d_hi = base - 1;

            int64_t cmin = (L + rep - 1) / rep;
            int64_t cmax = R / rep;

            cmin = std::max(cmin, d_lo);
            cmax = std::min(cmax, d_hi);
            if (cmin > cmax) continue;

            for (int64_t d = cmin; d <= cmax; ++d) {
                sum += d * rep;
            }
        }
    }

    return std::to_string(sum);
}

// ------------------------------------------------------------
// Part 2
// ------------------------------------------------------------

std::string Day02::part2() {
    int64_t total = 0;

    for (auto [L, R] : ranges_) {
        int max_digits = static_cast<int>(std::to_string(R).size());

        for (int total_digits = 2; total_digits <= max_digits; ++total_digits) {
            int64_t ten_len = POW10[total_digits];

            for (int m = 2; m <= total_digits; ++m) {
                if (total_digits % m != 0) continue;

                int k = total_digits / m;
                int64_t base_k = POW10[k];
                int64_t rep = (ten_len - 1) / (base_k - 1);

                int64_t d_lo = POW10[k - 1];
                int64_t d_hi = base_k - 1;

                int64_t cmin = (L + rep - 1) / rep;
                int64_t cmax = R / rep;

                cmin = std::max(cmin, d_lo);
                cmax = std::min(cmax, d_hi);
                if (cmin > cmax) continue;

                for (int64_t d = cmin; d <= cmax; ++d) {
                    std::string ds = std::to_string(d);
                    if (smallest_block(ds) != static_cast<int>(ds.size()))
                        continue;
                    total += d * rep;
                }
            }
        }
    }

    return std::to_string(total);
}