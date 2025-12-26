#pragma once

#include "core/Solution.h"

#include <cstdint>
#include <string>
#include <vector>

class Day08 final : public Solution {
public:
    void set_input(const std::vector<std::string>& lines) override;
    std::string part1() override;
    std::string part2() override;

    struct Vec3 {
        int64_t x, y, z;
    };

    struct Edge {
        int64_t dist2;
        int i, j;
    };

    std::vector<Vec3> points;
    std::vector<Edge> edges;

    // Helpers
    static int64_t squared_dist(const Vec3& a, const Vec3& b);
    static std::vector<Edge> build_sorted_edges(const std::vector<Vec3>& pts);

    // DSU
    struct DSU {
        std::vector<int> parent;
        std::vector<int> size;

        explicit DSU(int n);
        int find(int x);
        bool unite(int a, int b);
    };

    static std::vector<int> run_connections(
        const std::vector<Vec3>& points,
        const std::vector<Edge>& edges,
        int k
    );

    static std::pair<int,int> run_until_single_circuit(
        const std::vector<Vec3>& points,
        const std::vector<Edge>& edges
    );
};