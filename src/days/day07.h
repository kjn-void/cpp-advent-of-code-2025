#pragma once

#include "core/Solution.h"

#include <cstdint>
#include <string>
#include <vector>

class Day07 final : public Solution {
  public:
    void set_input(const std::vector<std::string>& input_lines) override;
    std::string part1() override;
    std::string part2() override;

  private:
    std::vector<std::string> manifold_;
    int row_count_ = 0;
    int column_count_ = 0;
    int start_column_ = -1;
};
