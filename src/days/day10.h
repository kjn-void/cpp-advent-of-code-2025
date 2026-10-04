#pragma once

#include <cstdint>

#include "core/Solution.h"
#include <string>
#include <string_view>
#include <vector>

class Day10 final : public Slv {
  public:
    void SetInput(const std::vector<std::string>& rgusLines) override;
    std::string TxtPart1() override;
    std::string TxtPart2() override;

  private:
    struct Mch {
        std::vector<int> rgfDiagram;
        std::vector<int> jvRequired;
        std::vector<std::vector<int>> rgbtn;
    };

    std::vector<Mch> rgmch_;

    // solvers
    static int CprSolveLights(const Mch& mch);
    static std::int64_t CprSolveJoltage(const Mch& mch);

    // parsing helpers
    static std::vector<int> RgvalParseList(std::string_view usList);
};
