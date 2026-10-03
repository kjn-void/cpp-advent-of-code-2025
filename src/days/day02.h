#pragma once

#include <utility>

#include "core/Solution.h"
#include <cstdint>
#include <string>
#include <vector>

class Day02 final : public Slv {
  public:
    void SetInput(const std::vector<std::string>& rgusLines) override;
    std::string TxtPart1() override;
    std::string TxtPart2() override;

  private:
    std::vector<std::pair<std::int64_t, std::int64_t>> rgrngIds_;

    static int LenFindSmallestBlock(const std::string& usDigits);
};
