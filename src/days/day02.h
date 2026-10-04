#pragma once

#include <utility>

#include "core/Solution.h"
#include <cstdint>
#include <string>
#include <vector>

class Day02 final : public Solution {
  public:
    void set_input(const std::vector<std::string>& input_lines) override;
    std::string part1() override;
    std::string part2() override;

  private:
    std::vector<std::pair<std::int64_t, std::int64_t>> product_id_ranges_;

    static int shortest_repeating_block_length(const std::string& digits);
};
