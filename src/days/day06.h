#pragma once

#include <span>

#include "core/Solution.h"
#include <cstdint>
#include <string>
#include <vector>

class Day06 final : public Slv {
  public:
    void SetInput(const std::vector<std::string>& rgusLines) override;
    std::string TxtPart1() override;
    std::string TxtPart2() override;

  private:
    struct Prb {
        int colFirst;
        int colLast;
    };

    std::vector<std::string> gridWorksheet_;
    int crw_ = 0;
    int ccol_ = 0;

    std::vector<Prb> RgprbFindProblems() const;
    char ChProblemOperator(const Prb& prb) const;

    std::vector<std::int64_t> RgvalReadRows(const Prb& prb) const;
    std::vector<std::int64_t> RgvalReadColumns(const Prb& prb) const;

    template <typename Fn> std::int64_t ValGrandTotal(Fn&& fnReadNumbers) const;

    static std::int64_t ValEvaluateProblem(std::span<const std::int64_t> rgvalOperands,
                                           char chOperator);
};
