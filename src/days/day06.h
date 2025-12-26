#pragma once

#include "core/Solution.h"
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

class Day06 final : public Solution {
public:
    void set_input(const std::vector<std::string>& lines) override;
    std::string part1() override;
    std::string part2() override;

private:
    struct Block {
        int start;
        int end;
    };

    std::vector<std::string> grid_;
    int R_ = 0;
    int C_ = 0;

    std::vector<Block> find_blocks() const;
    char get_operator(const Block& b) const;

    std::vector<int64_t> extract_part1(const Block& b) const;
    std::vector<int64_t> extract_part2(const Block& b) const;

    int64_t evaluate_blocks(
        const std::function<std::vector<int64_t>(const Block&)>& extractor
    ) const;

    static int64_t eval_numbers(const std::vector<int64_t>& nums, char op);
};