#pragma once

#include <utility>

#include "core/Solution.h"
#include <cstdint>
#include <string>
#include <vector>

class Day02 final : public Solution {
  public:
    void set_input(const std::vector<std::string>& lines) override;
    std::string part1() override;
    std::string part2() override;

  private:
    std::vector<std::pair<std::int64_t, std::int64_t>> ranges_;

    static int smallest_block(const std::string& s);
};
