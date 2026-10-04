#pragma once

#include <cstdint>

#include "core/Solution.h"
#include <string>
#include <utility>
#include <vector>

class Day05 final : public Solution {
  public:
    void SetInput(const std::vector<std::string>& vectorInputLines) override;
    std::string Part1() override;
    std::string Part2() override;

  private:
    std::vector<std::pair<std::int64_t, std::int64_t>> m_vectorFreshIdRanges;
    std::vector<std::int64_t> m_vectorAvailableIngredientIds;

    bool IsFresh(std::int64_t iIngredientId) const;
};
