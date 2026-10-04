#pragma once

#include <cstdint>

#include "core/Solution.h"
#include <string>
#include <string_view>
#include <vector>

class Day10 final : public Solution {
  public:
    void set_input(const std::vector<std::string>& input_lines) override;
    std::string part1() override;
    std::string part2() override;

  private:
    struct MachineDefinition {
        std::vector<int> light_diagram;
        std::vector<int> joltage_requirements;
        std::vector<std::vector<int>> button_wirings;
    };

    std::vector<MachineDefinition> machines_;

    // solvers
    static int fewest_presses_for_lights(const MachineDefinition& machine);
    static std::int64_t fewest_presses_for_joltage(const MachineDefinition& machine);

    // parsing helpers
    static std::vector<int> parse_integer_list(std::string_view list_text);
};
