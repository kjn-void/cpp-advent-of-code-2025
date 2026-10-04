#pragma once

#include "core/Solution.h"
#include <string>
#include <vector>

class Day04 final : public Solution {
  public:
    void set_input(const std::vector<std::string>& input_lines) override;
    std::string part1() override;
    std::string part2() override;

  private:
    std::vector<std::string> paper_rolls_;
    int row_count_ = 0;
    int column_count_ = 0;

    // helpers
    int count_adjacent_rolls(int row, int column) const;
};
