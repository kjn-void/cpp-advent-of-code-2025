#pragma once

#include <cstdint>

#include "core/Solution.h"
#include <string>
#include <vector>

class Day09 final : public Solution {
  public:
    void set_input(const std::vector<std::string>& lines) override;
    std::string part1() override;
    std::string part2() override;

  private:
    struct Pt {
        int x, y;
    };

    std::vector<Pt> reds;

    static std::int64_t max_area_inclusive(const std::vector<Pt>& points);
};
