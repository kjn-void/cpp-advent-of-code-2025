#pragma once

#include <cstdint>

#include "core/Solution.h"
#include <string>
#include <vector>

class Day09 final : public Solution {
  public:
    void SetInput(const std::vector<std::string>& vectorInputLines) override;
    std::string Part1() override;
    std::string Part2() override;

  private:
    struct Tile {
        int m_iX, m_iY;
    };

    std::vector<Tile> m_vectorRedTiles;

    static std::int64_t LargestRectangleArea(const std::vector<Tile>& vectorRedTiles);
};
