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

static constexpr std::array<std::uint64_t, 20> make_powers_of_ten() {
    std::array<std::uint64_t, 20> powers{};
    std::uint64_t power_of_ten = 1;
    for (std::size_t exponent = 0; exponent < powers.size(); ++exponent) {
        powers[exponent] = power_of_ten;
        if (exponent + 1 < powers.size())
            power_of_ten *= 10;
    }
    return powers;
}

static constexpr auto powers_of_ten = make_powers_of_ten();

int Day02::shortest_repeating_block_length(const std::string& digits) {
    const int digit_count = static_cast<int>(digits.size());
    for (int block_length = 1; block_length <= digit_count / 2; ++block_length) {
        if (digit_count % block_length != 0)
            continue;

        const std::string_view digit_block{digits.data(), static_cast<std::size_t>(block_length)};
        bool repeats = true;

        for (int block_offset = block_length; block_offset < digit_count;
             block_offset += block_length) {
            if (std::string_view{digits.data() + block_offset,
                                 static_cast<std::size_t>(block_length)} != digit_block) {
                repeats = false;
                break;
            }
        }

        if (repeats)
            return block_length;
    }
    return digit_count;
}

// ------------------------------------------------------------
// Input
// ------------------------------------------------------------

void Day02::set_input(const std::vector<std::string>& input_lines) {
    product_id_ranges_.clear();
    if (input_lines.empty())
        return;

    const auto line = core::trim(input_lines.front());
    if (!line.empty() && line.back() == ',')
        throw std::invalid_argument("Trailing comma in ID ranges");
    std::size_t range_offset = 0;

    while (range_offset < line.size()) {
        std::size_t comma_offset = line.find(',', range_offset);
        if (comma_offset == std::string::npos)
            comma_offset = line.size();

        std::string_view id_range_text(line.data() + range_offset, comma_offset - range_offset);
        std::size_t dash_offset = id_range_text.find('-');

        if (dash_offset == std::string_view::npos)
            throw std::invalid_argument("Expected an ID range");
        const auto first_id =
            core::parse_integer<std::int64_t>(id_range_text.substr(0, dash_offset));
        const auto last_id =
            core::parse_integer<std::int64_t>(id_range_text.substr(dash_offset + 1));
        if (first_id < 0 || last_id < first_id)
            throw std::invalid_argument("Invalid ID range");

        product_id_ranges_.emplace_back(first_id, last_id);
        range_offset = comma_offset + 1;
    }
}

// ------------------------------------------------------------
// Part 1
// ------------------------------------------------------------

std::string Day02::part1() {
    std::int64_t invalid_id_sum = 0;

    for (auto [first_id, last_id] : product_id_ranges_) {
        int max_digit_count = static_cast<int>(std::to_string(last_id).size());

        for (int block_length = 1; 2 * block_length <= max_digit_count; ++block_length) {
            std::int64_t block_base = powers_of_ten[block_length];
            std::int64_t repetition_factor = block_base + 1;

            std::int64_t smallest_block = powers_of_ten[block_length - 1];
            std::int64_t largest_block = block_base - 1;

            std::int64_t first_block =
                first_id / repetition_factor + (first_id % repetition_factor != 0);
            std::int64_t last_block = last_id / repetition_factor;

            first_block = std::max(first_block, smallest_block);
            last_block = std::min(last_block, largest_block);
            if (first_block > last_block)
                continue;

            // Sum the arithmetic progression without enumerating every repeated ID.
            auto block_count = last_block - first_block + 1;
            auto endpoint_sum = first_block + last_block;
            if (block_count % 2 == 0)
                block_count /= 2;
            else
                endpoint_sum /= 2;
            const auto remaining_sum_capacity =
                std::numeric_limits<std::int64_t>::max() - invalid_id_sum;
            if (endpoint_sum > remaining_sum_capacity / repetition_factor / block_count)
                throw std::overflow_error("ID sum exceeds int64_t");
            invalid_id_sum += endpoint_sum * block_count * repetition_factor;
        }
    }

    return std::to_string(invalid_id_sum);
}

// ------------------------------------------------------------
// Part 2
// ------------------------------------------------------------

std::string Day02::part2() {
    std::int64_t invalid_id_sum = 0;

    for (auto [first_id, last_id] : product_id_ranges_) {
        int max_digit_count = static_cast<int>(std::to_string(last_id).size());

        for (int id_digit_count = 2; id_digit_count <= max_digit_count; ++id_digit_count) {
            const auto id_digit_base = powers_of_ten[id_digit_count];

            for (int repetition_count = 2; repetition_count <= id_digit_count; ++repetition_count) {
                if (id_digit_count % repetition_count != 0)
                    continue;

                int block_length = id_digit_count / repetition_count;
                std::int64_t block_base = powers_of_ten[block_length];
                const auto repetition_factor =
                    static_cast<std::int64_t>((id_digit_base - 1) / (block_base - 1));

                std::int64_t smallest_block = powers_of_ten[block_length - 1];
                std::int64_t largest_block = block_base - 1;

                std::int64_t first_block =
                    first_id / repetition_factor + (first_id % repetition_factor != 0);
                std::int64_t last_block = last_id / repetition_factor;

                first_block = std::max(first_block, smallest_block);
                last_block = std::min(last_block, largest_block);
                if (first_block > last_block)
                    continue;

                for (std::int64_t block = first_block; block <= last_block; ++block) {
                    std::string block_text = std::to_string(block);
                    if (shortest_repeating_block_length(block_text) !=
                        static_cast<int>(block_text.size()))
                        continue;
                    const auto repeated_id = block * repetition_factor;
                    if (repeated_id > std::numeric_limits<std::int64_t>::max() - invalid_id_sum)
                        throw std::overflow_error("ID sum exceeds int64_t");
                    invalid_id_sum += repeated_id;
                }
            }
        }
    }

    return std::to_string(invalid_id_sum);
}
