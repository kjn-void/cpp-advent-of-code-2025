#pragma once

#include "core/Solution.h"

#include <cstddef>
#include <string>
#include <vector>

class Day12 final : public Solution {
  public:
    void set_input(const std::vector<std::string>& input_lines) override;
    std::string part1() override;
    std::string part2() override;

  private:
    // ------------------------------------------------------------
    // Data types
    // ------------------------------------------------------------

    struct CellOffset {
        int column_offset, row_offset;
    };

    struct PresentOrientation {
        int width, height;
        std::vector<CellOffset> cell_offsets;
    };

    struct PresentShape {
        int occupied_area = 0;
        std::vector<PresentOrientation> orientations;
    };

    struct TreeRegion {
        int width, height;
        std::vector<int> present_counts;
    };

    std::vector<PresentShape> present_shapes_;
    std::vector<TreeRegion> tree_regions_;

    // ------------------------------------------------------------
    // Helpers
    // ------------------------------------------------------------

    static PresentShape make_present_shape(const std::vector<std::string>& shape_rows);
    static std::vector<std::vector<bool>>
    rotate_clockwise(const std::vector<std::vector<bool>>& grid);
    static std::vector<std::vector<bool>>
    reflect_horizontally(const std::vector<std::vector<bool>>& grid);
    static PresentOrientation grid_to_orientation(const std::vector<std::vector<bool>>& grid);
    static std::string orientation_key(const PresentOrientation& orientation);

    bool presents_fit(const TreeRegion& region) const;

    bool can_pack_region(const TreeRegion& region) const;
    bool place_remaining_presents(
        std::vector<bool>& occupied_cells, std::vector<int>& remaining_counts,
        const std::vector<std::vector<std::vector<std::size_t>>>& placements_by_shape,
        std::vector<std::size_t>& first_placement_by_shape) const;
};
