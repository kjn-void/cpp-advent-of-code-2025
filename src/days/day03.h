#pragma once

#include <span>

#include <cstdint>

#include "core/Solution.h"
#include <vector>

class Day03 final : public Solution {
  public:
    void SetInput(const std::vector<std::string>& vectorInputLines) override;
    std::string Part1() override;
    std::string Part2() override;

  private:
    std::vector<std::vector<int>> m_vectorBatteryBanks;

    std::string TotalOutputJoltage(int iBatteriesToSelect) const;
    static std::int64_t JoltageFromRatings(std::span<const int> spanSelectedRatings);
};
