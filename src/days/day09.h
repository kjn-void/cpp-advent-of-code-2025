#pragma once

#include "core/Solution.h"
#include <vector>
#include <string>

class Day09 final : public Solution {
public:
    void set_input(const std::vector<std::string>& lines) override;
    std::string part1() override;
    std::string part2() override;

private:
    struct Pt {
        int x, y;
    };

    struct Edge {
        int x1, y1;
        int x2, y2;
        bool hor;
    };

    std::vector<Pt> reds;
    std::vector<Edge> edges;

    // helpers
    static int max_area_inclusive(const std::vector<Pt>& pts);
    void build_edges();

    bool point_inside_or_on(const Pt& p) const;
    static bool point_in_polygon_ray_cast(const Pt& p, const std::vector<Pt>& poly);

    bool rectangle_cut_by_polygon(int x1, int y1, int x2, int y2) const;

    static int abs_int(int v) { return v < 0 ? -v : v; }
    static int min_int(int a, int b) { return a < b ? a : b; }
    static int max_int(int a, int b) { return a > b ? a : b; }
};