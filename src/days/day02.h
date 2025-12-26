#pragma once

#include "core/Solution.h"
#include <cstdint>
#include <vector>
#include <string>

class Day02 final : public Solution {
public:
    void set_input(const std::vector<std::string>& lines) override;
    std::string part1() override;
    std::string part2() override;

private:
    std::vector<std::pair<int64_t, int64_t>> ranges_;

    static int smallest_block(const std::string& s);
};