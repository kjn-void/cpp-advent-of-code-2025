#pragma once

#include <span>

#include "core/Solution.h"

#include <cstdint>
#include <string>
#include <vector>

class Day08 final : public Solution {
  public:
    void set_input(const std::vector<std::string>& input_lines) override;
    std::string part1() override;
    std::string part2() override;

    struct JunctionBox {
        std::int64_t x, y, z;
    };

    // Every pair of junction boxes, a candidate for connection.
    struct BoxPair {
        std::int64_t squared_distance;
        int first_box_index, second_box_index;
    };

    std::vector<JunctionBox> junction_boxes;
    std::vector<BoxPair> pairs_by_distance;

    // Helpers
    static std::int64_t squared_distance(const JunctionBox& first_box,
                                         const JunctionBox& second_box);
    static std::vector<BoxPair> sort_pairs_by_distance(std::span<const JunctionBox> junction_boxes);

    // Union-find
    struct CircuitSet {
        std::vector<int> parent_by_box;
        std::vector<int> box_count_by_root;

        explicit CircuitSet(int box_count);
        int find_circuit_root(int box_index);
        bool join_circuits(int first_box_index, int second_box_index);
    };

    static std::vector<int>
    circuit_sizes_after_connections(std::span<const JunctionBox> junction_boxes,
                                    std::span<const BoxPair> pairs_by_distance,
                                    int connection_count);

    static BoxPair connect_all_junction_boxes(std::span<const JunctionBox> junction_boxes,
                                              std::span<const BoxPair> pairs_by_distance);
};
