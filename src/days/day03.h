#pragma once

#include <span>

#include <cstdint>

#include "core/Solution.h"
#include <vector>

class Day03 final : public Solution {
  public:
    void set_input(const std::vector<std::string>& input_lines) override;
    std::string part1() override;
    std::string part2() override;

  private:
    std::vector<std::vector<int>> battery_banks_;

    std::string total_output_joltage(int batteries_to_select) const;
    static std::int64_t joltage_from_ratings(std::span<const int> selected_ratings);
};
