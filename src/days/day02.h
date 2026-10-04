#pragma once

#include <utility>

#include "core/Solution.h"
#include <cstdint>
#include <string>
#include <vector>

class Day02 final : public Solution {
  public:
    void SetInput(const std::vector<std::string>& vectorInputLines) override;
    std::string Part1() override;
    std::string Part2() override;

  private:
    std::vector<std::pair<std::int64_t, std::int64_t>> m_vectorProductIdRanges;

    static int ShortestRepeatingBlockLength(const std::string& stringDigits);
};
