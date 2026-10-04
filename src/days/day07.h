#pragma once

#include "core/Solution.h"

#include <cstdint>
#include <string>
#include <vector>

class Day07 final : public Solution {
  public:
    void SetInput(const std::vector<std::string>& vectorInputLines) override;
    std::string Part1() override;
    std::string Part2() override;

  private:
    std::vector<std::string> m_vectorManifold;
    int m_iRowCount = 0;
    int m_iColumnCount = 0;
    int m_iStartColumn = -1;
};
