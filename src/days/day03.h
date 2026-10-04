#pragma once

#include <span>

#include <cstdint>

#include "core/Solution.h"
#include <vector>

class Day03 final : public Slv {
  public:
    void SetInput(const std::vector<std::string>& rgusLines) override;
    std::string TxtPart1() override;
    std::string TxtPart2() override;

  private:
    std::vector<std::vector<int>> rgbnk_;

    std::string TxtTotalJoltage(int cbatToSelect) const;
    static std::int64_t JolFromRatings(std::span<const int> rgbatSelected);
};
