#pragma once

#include "core/Solution.h"
#include <vector>
#include <string>
#include <utility>

class Day05 final : public Solution {
public:
    void set_input(const std::vector<std::string>& lines) override;
    std::string part1() override;
    std::string part2() override;

private:
    std::vector<std::pair<int64_t, int64_t>> ranges_;
    std::vector<int64_t> ids_;

    bool is_fresh(int64_t id) const;
};