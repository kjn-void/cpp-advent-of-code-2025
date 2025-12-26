#pragma once
#include <string>
#include <vector>

struct Solution {
    virtual ~Solution() = default;
    virtual void set_input(const std::vector<std::string>& lines) = 0;
    virtual std::string part1() = 0;
    virtual std::string part2() = 0;
};