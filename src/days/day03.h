#pragma once

#include <span>

#include <cstdint>

#include "core/Solution.h"
#include <vector>

class Day03 final : public Solution {
  public:
    void set_input(const std::vector<std::string>& lines) override;
    std::string part1() override;
    std::string part2() override;

  private:
    std::vector<std::vector<int>> banks;

    std::string max_joltage(int pick) const;
    static std::int64_t stack_to_number(std::span<const int> stack);
};
