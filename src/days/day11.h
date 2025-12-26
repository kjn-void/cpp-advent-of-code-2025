#pragma once

#include "core/Solution.h"

#include <string>
#include <vector>
#include <unordered_map>

class Day11 final : public Solution {
public:
    void set_input(const std::vector<std::string>& lines) override;
    std::string part1() override;
    std::string part2() override;

private:
    // adjacency list
    std::unordered_map<std::string, std::vector<std::string>> adj;

    // ---------- Part 1 ----------
    int64_t count_paths_from(
        const std::string& node,
        std::unordered_map<std::string, int64_t>& memo,
        std::unordered_map<std::string, bool>& visiting
    );

    // ---------- Part 2 ----------
    struct State {
        std::string node;
        int mask;

        bool operator==(const State& other) const {
            return node == other.node && mask == other.mask;
        }
    };

    struct StateHash {
        std::size_t operator()(const State& s) const {
            return std::hash<std::string>()(s.node) ^ (std::hash<int>()(s.mask) << 1);
        }
    };

    int64_t count_paths_with_required(
        const std::string& start,
        const std::string& end,
        const std::string& need1,
        const std::string& need2
    );
};