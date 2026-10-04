#pragma once

#include "core/Solution.h"
#include <string>
#include <vector>

class Day04 final : public Slv {
  public:
    void SetInput(const std::vector<std::string>& rgusLines) override;
    std::string TxtPart1() override;
    std::string TxtPart2() override;

  private:
    std::vector<std::string> gridRolls_;
    int crw_ = 0;
    int ccol_ = 0;

    // helpers
    int CrolAdjacent(int rw, int col) const;
};
