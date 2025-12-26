#pragma once

#include "core/Solution.h"

#include <string>
#include <vector>
#include <cstdint>

class Day07 final : public Solution {
public:
    void set_input(const std::vector<std::string>& lines) override;
    std::string part1() override;
    std::string part2() override;

private:
    std::vector<std::string> grid_;
    int rows_ = 0;
    int cols_ = 0;
    int start_col_ = 0;
};