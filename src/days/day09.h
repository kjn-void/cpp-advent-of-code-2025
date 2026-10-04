#pragma once

#include <cstdint>

#include "core/Solution.h"
#include <string>
#include <vector>

class Day09 final : public Solution {
  public:
    void set_input(const std::vector<std::string>& input_lines) override;
    std::string part1() override;
    std::string part2() override;

  private:
    struct Tile {
        int x, y;
    };

    std::vector<Tile> red_tiles_;

    static std::int64_t largest_rectangle_area(const std::vector<Tile>& red_tiles);
};
