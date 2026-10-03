#include "days/day02.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <array>
#include <limits>
#include <stdexcept>

// ------------------------------------------------------------
// Registration
// ------------------------------------------------------------
namespace {
const core::DayRegistration<Day02> registration{2};
} // namespace

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

static constexpr std::array<std::uint64_t, 20> pow10_table() {
    std::array<std::uint64_t, 20> t{};
    std::uint64_t x = 1;
    for (std::size_t i = 0; i < t.size(); ++i) {
        t[i] = x;
        if (i + 1 < t.size())
            x *= 10;
    }
    return t;
}

static constexpr auto POW10 = pow10_table();

int Day02::smallest_block(const std::string& s) {
    const int n = static_cast<int>(s.size());
    for (int k = 1; k <= n / 2; ++k) {
        if (n % k != 0)
            continue;

        const std::string_view block{s.data(), static_cast<std::size_t>(k)};
        bool ok = true;

        for (int i = k; i < n; i += k) {
            if (std::string_view{s.data() + i, static_cast<std::size_t>(k)} != block) {
                ok = false;
                break;
            }
        }

        if (ok)
            return k;
    }
    return n;
}

// ------------------------------------------------------------
// Input
// ------------------------------------------------------------

void Day02::set_input(const std::vector<std::string>& lines) {
    ranges_.clear();
    if (lines.empty())
        return;

    const auto line = core::trim(lines.front());
    if (!line.empty() && line.back() == ',')
        throw std::invalid_argument("Trailing comma in ID ranges");
    std::size_t pos = 0;

    while (pos < line.size()) {
        std::size_t comma = line.find(',', pos);
        if (comma == std::string::npos)
            comma = line.size();

        std::string_view part(line.data() + pos, comma - pos);
        std::size_t dash = part.find('-');

        if (dash == std::string_view::npos)
            throw std::invalid_argument("Expected an ID range");
        const auto lo = core::parse_integer<std::int64_t>(part.substr(0, dash));
        const auto hi = core::parse_integer<std::int64_t>(part.substr(dash + 1));
        if (lo < 0 || hi < lo)
            throw std::invalid_argument("Invalid ID range");

        ranges_.emplace_back(lo, hi);
        pos = comma + 1;
    }
}

// ------------------------------------------------------------
// Part 1
// ------------------------------------------------------------

std::string Day02::part1() {
    std::int64_t sum = 0;

    for (auto [L, R] : ranges_) {
        int max_digits = static_cast<int>(std::to_string(R).size());

        for (int k = 1; 2 * k <= max_digits; ++k) {
            std::int64_t base = POW10[k];
            std::int64_t rep = base + 1;

            std::int64_t d_lo = POW10[k - 1];
            std::int64_t d_hi = base - 1;

            std::int64_t cmin = L / rep + (L % rep != 0);
            std::int64_t cmax = R / rep;

            cmin = std::max(cmin, d_lo);
            cmax = std::min(cmax, d_hi);
            if (cmin > cmax)
                continue;

            // Sum the arithmetic progression without enumerating every repeated ID.
            auto count = cmax - cmin + 1;
            auto endpoints = cmin + cmax;
            if (count % 2 == 0)
                count /= 2;
            else
                endpoints /= 2;
            const auto available = std::numeric_limits<std::int64_t>::max() - sum;
            if (endpoints > available / rep / count)
                throw std::overflow_error("ID sum exceeds int64_t");
            sum += endpoints * count * rep;
        }
    }

    return std::to_string(sum);
}

// ------------------------------------------------------------
// Part 2
// ------------------------------------------------------------

std::string Day02::part2() {
    std::int64_t total = 0;

    for (auto [L, R] : ranges_) {
        int max_digits = static_cast<int>(std::to_string(R).size());

        for (int total_digits = 2; total_digits <= max_digits; ++total_digits) {
            const auto ten_len = POW10[total_digits];

            for (int m = 2; m <= total_digits; ++m) {
                if (total_digits % m != 0)
                    continue;

                int k = total_digits / m;
                std::int64_t base_k = POW10[k];
                const auto rep = static_cast<std::int64_t>((ten_len - 1) / (base_k - 1));

                std::int64_t d_lo = POW10[k - 1];
                std::int64_t d_hi = base_k - 1;

                std::int64_t cmin = L / rep + (L % rep != 0);
                std::int64_t cmax = R / rep;

                cmin = std::max(cmin, d_lo);
                cmax = std::min(cmax, d_hi);
                if (cmin > cmax)
                    continue;

                for (std::int64_t d = cmin; d <= cmax; ++d) {
                    std::string ds = std::to_string(d);
                    if (smallest_block(ds) != static_cast<int>(ds.size()))
                        continue;
                    const auto value = d * rep;
                    if (value > std::numeric_limits<std::int64_t>::max() - total)
                        throw std::overflow_error("ID sum exceeds std::int64_t");
                    total += value;
                }
            }
        }
    }

    return std::to_string(total);
}
