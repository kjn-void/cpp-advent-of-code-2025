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
    struct Tl {
        int x, y;
    };

    std::vector<Tl> rgtlRed_;

    static std::int64_t AreaLargestRectangle(const std::vector<Tl>& rgtl);
};
