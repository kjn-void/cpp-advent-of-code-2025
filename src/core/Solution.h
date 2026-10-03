#pragma once
#include <string>
#include <vector>

struct Slv {
    virtual ~Slv() = default;
    virtual void SetInput(const std::vector<std::string>& rgusLines) = 0;
    virtual std::string TxtPart1() = 0;
    virtual std::string TxtPart2() = 0;
};
