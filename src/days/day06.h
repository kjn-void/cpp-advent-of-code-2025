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
    struct Blk {
        int colFirst;
        int colLast;
    };

    std::vector<std::string> gridWorksheet_;
    int crw_ = 0;
    int ccol_ = 0;

    std::vector<Blk> RgblkFind() const;
    char ChGetOperator(const Blk& blk) const;

    std::vector<std::int64_t> RgvalExtractPart1(const Blk& blk) const;
    std::vector<std::int64_t> RgvalExtractPart2(const Blk& blk) const;

    template <typename Fn> std::int64_t ValEvaluateBlocks(Fn&& fnExtractOperands) const;

    static std::int64_t ValEvaluateOperands(std::span<const std::int64_t> rgvalOperands,
                                            char chOperator);
};
