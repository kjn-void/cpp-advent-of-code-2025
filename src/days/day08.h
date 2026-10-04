#pragma once

#include <span>

#include <utility>

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

    struct Connection {
        std::int64_t squared_distance;
        int first_box_index, second_box_index;
    };

    std::vector<JunctionBox> junction_boxes;
    std::vector<Connection> connections;

    // Helpers
    static std::int64_t squared_distance(const JunctionBox& first_box,
                                         const JunctionBox& second_box);
    static std::vector<Connection> sorted_connections(std::span<const JunctionBox> junction_boxes);

    // DSU
    struct CircuitSet {
        std::vector<int> parent_by_box;
        std::vector<int> box_count_by_root;

        explicit CircuitSet(int box_count);
        int find_circuit(int circuit_root);
        bool join_circuits(int first_root, int second_root);
    };

    static std::vector<int>
    circuit_sizes_after_connections(std::span<const JunctionBox> junction_boxes,
                                    std::span<const Connection> connections, int connection_limit);

    static std::pair<int, int>
    connect_all_junction_boxes(std::span<const JunctionBox> junction_boxes,
                               std::span<const Connection> connections);
};
