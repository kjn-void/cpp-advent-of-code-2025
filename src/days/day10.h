#pragma once

#include <cstdint>

#include "core/Solution.h"
#include <string>
#include <string_view>
#include <vector>

class Day10 final : public Solution {
  public:
    void SetInput(const std::vector<std::string>& vectorInputLines) override;
    std::string Part1() override;
    std::string Part2() override;

  private:
    struct MachineDefinition {
        std::vector<int> m_vectorLightDiagram;
        std::vector<int> m_vectorJoltageRequirements;
        std::vector<std::vector<int>> m_vectorButtonWirings;
    };

    std::vector<MachineDefinition> m_vectorMachines;

    // solvers
    static int FewestPressesForLights(const MachineDefinition& machinedefinition);
    static std::int64_t FewestPressesForJoltage(const MachineDefinition& machinedefinition);

    // parsing helpers
    static std::vector<int> ParseIntegerList(std::string_view stringList);
};
