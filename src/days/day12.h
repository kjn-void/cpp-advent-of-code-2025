#pragma once

#include "core/Solution.h"

#include <cstddef>
#include <string>
#include <vector>

class Day12 final : public Solution {
  public:
    void set_input(const std::vector<std::string>& lines) override;
    std::string part1() override;
    std::string part2() override;

  private:
    // ------------------------------------------------------------
    // Data types
    // ------------------------------------------------------------

    struct Point {
        int x, y;
    };

    struct Variant {
        int width, height;
        std::vector<Point> cells;
    };

    struct Shape {
        int area = 0;
        std::vector<Variant> variants;
    };

    struct Region {
        int width, height;
        std::vector<int> counts;
    };

    std::vector<Shape> shapes;
    std::vector<Region> regions;

    // ------------------------------------------------------------
    // Helpers
    // ------------------------------------------------------------

    static Shape build_shape(const std::vector<std::string>& rows);
    static std::vector<std::vector<bool>> rotate_grid(const std::vector<std::vector<bool>>& g);
    static std::vector<std::vector<bool>> flip_grid_h(const std::vector<std::vector<bool>>& g);
    static Variant grid_to_variant(const std::vector<std::vector<bool>>& g);
    static std::string variant_key(const Variant& v);

    bool region_can_fit(const Region& r) const;

    bool can_pack_region(const Region& r) const;
    bool pack(std::vector<bool>& board, std::vector<int>& counts,
              const std::vector<std::vector<std::vector<std::size_t>>>& placements,
              std::vector<std::size_t>& first_placement) const;
};
