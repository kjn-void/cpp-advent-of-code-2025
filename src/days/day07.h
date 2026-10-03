#pragma once

#include "core/Solution.h"

#include <cstdint>
#include <string>
#include <vector>

class Day07 final : public Slv {
  public:
    void SetInput(const std::vector<std::string>& rgusLines) override;
    std::string TxtPart1() override;
    std::string TxtPart2() override;

  private:
    std::vector<std::string> gridManifold_;
    int crw_ = 0;
    int ccol_ = 0;
    int colStart_ = -1;
};
