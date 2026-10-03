#pragma once

#include <cstdint>

#include "core/Solution.h"
#include <string>
#include <vector>

class Day09 final : public Slv {
  public:
    void SetInput(const std::vector<std::string>& rgusLines) override;
    std::string TxtPart1() override;
    std::string TxtPart2() override;

  private:
    struct Pt {
        int xTile, yTile;
    };

    std::vector<Pt> rgptRed_;

    static std::int64_t AreaMaxInclusive(const std::vector<Pt>& rgpt);
};
