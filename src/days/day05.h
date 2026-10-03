#pragma once

#include <cstdint>

#include "core/Solution.h"
#include <string>
#include <utility>
#include <vector>

class Day05 final : public Slv {
  public:
    void SetInput(const std::vector<std::string>& rgusLines) override;
    std::string TxtPart1() override;
    std::string TxtPart2() override;

  private:
    std::vector<std::pair<std::int64_t, std::int64_t>> rgrngFresh_;
    std::vector<std::int64_t> rgidIngredients_;

    bool FIsFresh(std::int64_t idIngredient) const;
};
