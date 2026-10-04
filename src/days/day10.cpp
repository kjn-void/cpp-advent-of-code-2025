#include "days/day10.h"
#include "core/Parallel.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

// Registration
namespace {
const core::DayRegistration<Day10> registration{10};
} // namespace

// ------------------------------------------------------------
// Parsing helpers
// ------------------------------------------------------------

std::vector<int> Day10::parse_integer_list(std::string_view list_text) {
    std::vector<int> integers;
    if (list_text.size() < 2)
        return integers;

    const auto list_contents = list_text.substr(1, list_text.size() - 2);
    std::string integer_text;
    std::istringstream list_input(std::string{list_contents});

    while (std::getline(list_input, integer_text, ',')) {
        integers.push_back(core::parse_integer<int>(integer_text));
    }
    if (!list_contents.empty() && list_contents.back() == ',')
        throw std::invalid_argument("Trailing comma in machine list");
    return integers;
}

void Day10::set_input(const std::vector<std::string>& input_lines) {
    machines_.clear();

    for (const auto& line : input_lines) {
        if (line.empty())
            continue;

        // lights
        auto lights_open = line.find('[');
        auto lights_close = line.find(']');
        if (lights_open == std::string::npos || lights_close == std::string::npos ||
            lights_close <= lights_open)
            throw std::invalid_argument("Missing machine lights");

        std::vector<int> light_diagram;
        for (char light_symbol : line.substr(lights_open + 1, lights_close - lights_open - 1)) {
            if (light_symbol != '#' && light_symbol != '.')
                throw std::invalid_argument("Invalid light state");
            light_diagram.push_back(light_symbol == '#' ? 1 : 0);
        }

        // joltage
        std::vector<int> joltage_requirements;
        auto joltage_open = line.find('{');
        auto joltage_close = line.find('}');
        if (joltage_open == std::string::npos || joltage_close == std::string::npos ||
            joltage_open <= lights_close || joltage_close <= joltage_open)
            throw std::invalid_argument("Missing machine joltage");
        joltage_requirements =
            parse_integer_list(line.substr(joltage_open, joltage_close - joltage_open + 1));

        // buttons
        std::vector<std::vector<int>> button_wirings;
        auto wiring_text = core::trim(
            std::string_view(line).substr(lights_close + 1, joltage_open - lights_close - 1));

        std::size_t button_open = 0;
        while (!wiring_text.empty()) {
            if (wiring_text.front() != '(')
                throw std::invalid_argument("Expected a machine button");
            auto button_close = wiring_text.find(')', button_open);
            if (button_close == std::string::npos)
                throw std::invalid_argument("Unclosed machine button");
            button_wirings.push_back(parse_integer_list(
                wiring_text.substr(button_open, button_close - button_open + 1)));
            wiring_text = core::trim(wiring_text.substr(button_close + 1));
        }

        if (!core::trim(std::string_view(line).substr(0, lights_open)).empty() ||
            !core::trim(std::string_view(line).substr(joltage_close + 1)).empty())
            throw std::invalid_argument("Unexpected text around machine");
        if (light_diagram.empty() || light_diagram.size() != joltage_requirements.size() ||
            std::ranges::any_of(joltage_requirements,
                                [](int required_joltage) { return required_joltage < 0; }))
            throw std::invalid_argument("Invalid machine targets");
        // A wiring index names an indicator light in part 1 and a joltage counter in part 2.
        for (auto& wiring : button_wirings) {
            std::ranges::sort(wiring);
            if (std::ranges::any_of(wiring,
                                    [&](int wired_index) {
                                        return wired_index < 0 ||
                                               wired_index >=
                                                   static_cast<int>(light_diagram.size());
                                    }) ||
                std::adjacent_find(wiring.begin(), wiring.end()) != wiring.end())
                throw std::invalid_argument("Invalid machine button index");
        }
        // Repeated or empty buttons cannot improve a minimum-press solution.
        std::erase_if(button_wirings, [](const auto& wiring) { return wiring.empty(); });
        std::ranges::sort(button_wirings);
        button_wirings.erase(std::unique(button_wirings.begin(), button_wirings.end()),
                             button_wirings.end());
        machines_.push_back(
            {std::move(light_diagram), std::move(joltage_requirements), std::move(button_wirings)});
    }
}

// ------------------------------------------------------------
// Part 1 — GF(2) Gaussian elimination
// ------------------------------------------------------------

int Day10::fewest_presses_for_lights(const MachineDefinition& machine) {
    int light_count = static_cast<int>(machine.light_diagram.size());
    int button_count = static_cast<int>(machine.button_wirings.size());

    std::vector<std::vector<int>> light_equations(light_count,
                                                  std::vector<int>(button_count + 1, 0));
    for (int light_index = 0; light_index < light_count; ++light_index)
        light_equations[light_index][button_count] = machine.light_diagram[light_index];

    for (int button_column = 0; button_column < button_count; ++button_column)
        for (int light_index : machine.button_wirings[button_column])
            if (light_index < light_count)
                light_equations[light_index][button_column] = 1;

    int pivot_row = 0;
    std::vector<int> pivot_row_by_column(button_count, -1);

    for (int pivot_column = 0; pivot_column < button_count && pivot_row < light_count;
         ++pivot_column) {
        int selected_row = -1;
        for (int row = pivot_row; row < light_count; ++row) {
            if (light_equations[row][pivot_column]) {
                selected_row = row;
                break;
            }
        }
        if (selected_row == -1)
            continue;

        std::swap(light_equations[pivot_row], light_equations[selected_row]);
        pivot_row_by_column[pivot_column] = pivot_row;

        for (int row = 0; row < light_count; ++row) {
            if (row != pivot_row && light_equations[row][pivot_column]) {
                for (int coefficient_column = pivot_column; coefficient_column <= button_count;
                     ++coefficient_column)
                    light_equations[row][coefficient_column] ^=
                        light_equations[pivot_row][coefficient_column];
            }
        }
        pivot_row++;
    }

    for (int row = pivot_row; row < light_count; ++row) {
        if (light_equations[row][button_count] != 0)
            throw std::runtime_error("Unreachable light target");
    }

    std::vector<int> free_button_columns;
    for (int button_column = 0; button_column < button_count; ++button_column)
        if (pivot_row_by_column[button_column] == -1)
            free_button_columns.push_back(button_column);

    int minimum_press_count = button_count + 1;
    std::vector<int> press_parity_by_button(button_count, 0);
    const auto search_parities = [&](auto&& recurse, std::size_t free_column_index,
                                     int press_count) -> void {
        if (press_count >= minimum_press_count)
            return;
        if (free_column_index < free_button_columns.size()) {
            const int free_column = free_button_columns[free_column_index];
            press_parity_by_button[free_column] = 0;
            recurse(recurse, free_column_index + 1, press_count);
            press_parity_by_button[free_column] = 1;
            recurse(recurse, free_column_index + 1, press_count + 1);
            return;
        }
        for (int button_column = button_count - 1; button_column >= 0; --button_column) {
            if (pivot_row_by_column[button_column] == -1)
                continue;
            const int row = pivot_row_by_column[button_column];
            int press_parity = light_equations[row][button_count];
            for (int coefficient_column = button_column + 1; coefficient_column < button_count;
                 ++coefficient_column)
                press_parity ^= light_equations[row][coefficient_column] &
                                press_parity_by_button[coefficient_column];
            press_parity_by_button[button_column] = press_parity;
            press_count += press_parity;
        }
        minimum_press_count = std::min(minimum_press_count, press_count);
    };
    search_parities(search_parities, 0, 0);

    return minimum_press_count;
}

// ------------------------------------------------------------
// Part 2 — exact integer recursion on the binary digits of press counts
// ------------------------------------------------------------

std::int64_t Day10::fewest_presses_for_joltage(const MachineDefinition& machine) {
    struct CounterVectorHash {
        std::size_t operator()(const std::vector<int>& counter_values) const noexcept {
            std::size_t hash_value = 0;
            for (int counter_value : counter_values)
                hash_value ^= std::hash<int>{}(counter_value) + 0x9e3779b9U + (hash_value << 6) +
                              (hash_value >> 2);
            return hash_value;
        }
    };
    struct ParityChoice {
        std::vector<int> counter_increments;
        int press_count;
    };

    const auto counter_count = machine.joltage_requirements.size();
    std::unordered_map<std::vector<int>, std::vector<ParityChoice>, CounterVectorHash>
        choices_by_parity;
    std::vector<int> counter_increments(counter_count, 0);
    // Enumerate every set of buttons pressed an odd number of times.
    const auto enumerate_odd_presses = [&](auto&& recurse, std::size_t button_index,
                                           int press_count) -> void {
        if (button_index == machine.button_wirings.size()) {
            auto increment_parity = counter_increments;
            for (auto& counter_value : increment_parity)
                counter_value %= 2;
            choices_by_parity[increment_parity].push_back({counter_increments, press_count});
            return;
        }
        recurse(recurse, button_index + 1, press_count);
        for (int counter_index : machine.button_wirings[button_index])
            ++counter_increments[counter_index];
        recurse(recurse, button_index + 1, press_count + 1);
        for (int counter_index : machine.button_wirings[button_index])
            --counter_increments[counter_index];
    };
    enumerate_odd_presses(enumerate_odd_presses, 0, 0);

    using PressCountResult = std::optional<std::int64_t>;
    std::unordered_map<std::vector<int>, PressCountResult, CounterVectorHash>
        minimum_presses_by_remaining_joltage;
    const auto minimum_presses =
        [&](auto&& recurse, const std::vector<int>& remaining_joltage) -> PressCountResult {
        if (std::ranges::all_of(remaining_joltage,
                                [](int counter_value) { return counter_value == 0; }))
            return 0;
        if (const auto cached_press_count =
                minimum_presses_by_remaining_joltage.find(remaining_joltage);
            cached_press_count != minimum_presses_by_remaining_joltage.end())
            return cached_press_count->second;

        auto remaining_parity = remaining_joltage;
        for (auto& counter_value : remaining_parity)
            counter_value %= 2;
        const auto matching_choices = choices_by_parity.find(remaining_parity);
        PressCountResult minimum_press_count;
        if (matching_choices != choices_by_parity.end()) {
            for (const auto& choice : matching_choices->second) {
                std::vector<int> halved_joltage(counter_count);
                bool fits_remaining_joltage = true;
                for (std::size_t counter_index = 0; counter_index < counter_count;
                     ++counter_index) {
                    if (choice.counter_increments[counter_index] >
                        remaining_joltage[counter_index]) {
                        fits_remaining_joltage = false;
                        break;
                    }
                    halved_joltage[counter_index] = (remaining_joltage[counter_index] -
                                                     choice.counter_increments[counter_index]) /
                                                    2;
                }
                if (!fits_remaining_joltage)
                    continue;
                // Each press adds at most one to any counter.
                const auto press_count_lower_bound =
                    choice.press_count + 2 * std::int64_t{std::ranges::max(halved_joltage)};
                if (minimum_press_count && press_count_lower_bound >= *minimum_press_count)
                    continue;
                if (const auto remaining_press_count = recurse(recurse, halved_joltage)) {
                    const auto total_presses = choice.press_count + 2 * *remaining_press_count;
                    if (!minimum_press_count || total_presses < *minimum_press_count)
                        minimum_press_count = total_presses;
                }
            }
        }
        minimum_presses_by_remaining_joltage.emplace(remaining_joltage, minimum_press_count);
        return minimum_press_count;
    };

    // Any press vector is uniquely x = odd + 2 * rest. Matching target parity
    // makes (target - A * odd) / 2 an exact, smaller integer subproblem.
    const auto minimum_press_count = minimum_presses(minimum_presses, machine.joltage_requirements);
    if (!minimum_press_count)
        throw std::runtime_error("Unreachable joltage target");
    return *minimum_press_count;
}

// ------------------------------------------------------------
// Day interface
// ------------------------------------------------------------

std::string Day10::part1() {
    const std::int64_t total_presses = core::parallel_sum_indexed(
        machines_.size(), [&](std::size_t machine_index) -> std::int64_t {
            const auto& machine = machines_[machine_index];
            if (machine.light_diagram.empty())
                return 0;
            return static_cast<std::int64_t>(fewest_presses_for_lights(machine));
        });

    return std::to_string(total_presses);
}

std::string Day10::part2() {
    const std::int64_t total_presses = core::parallel_sum_indexed(
        machines_.size(), [&](std::size_t machine_index) -> std::int64_t {
            const auto& machine = machines_[machine_index];
            if (machine.joltage_requirements.empty())
                return 0;
            return static_cast<std::int64_t>(fewest_presses_for_joltage(machine));
        });

    return std::to_string(total_presses);
}
