#pragma once

#include "core/Solution.h"
#include <vector>
#include <string>

class Day10 final : public Solution {
public:
    void set_input(const std::vector<std::string>& lines) override;
    std::string part1() override;
    std::string part2() override;

private:
    struct MachineData {
        std::vector<int> targetLights;
        std::vector<int> targetJoltage;
        std::vector<std::vector<int>> buttons;
    };

    std::vector<MachineData> machines;

    // solvers
    static int solve_lights(const MachineData& m);
    static int solve_joltage(const MachineData& m);

    // parsing helpers
    static std::vector<int> parse_list(const std::string& s);
};