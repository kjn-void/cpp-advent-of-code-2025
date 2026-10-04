#pragma once
#include <string>
#include <vector>

struct Solution {
    virtual ~Solution() = default;
    virtual void SetInput(const std::vector<std::string>& vectorInputLines) = 0;
    virtual std::string Part1() = 0;
    virtual std::string Part2() = 0;
};
