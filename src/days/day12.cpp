#include "days/day12.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

// Registration
namespace {
const core::DayRegistration<Day12> registration{12};
} // namespace

// ------------------------------------------------------------
// Parsing
// ------------------------------------------------------------

void Day12::set_input(const std::vector<std::string>& input_lines) {
    present_shapes_.clear();
    tree_regions_.clear();

    for (std::size_t line_index = 0; line_index < input_lines.size();) {
        const auto line = core::trim(input_lines[line_index++]);
        if (line.empty())
            continue;
        const auto colon_offset = line.find(':');
        if (colon_offset == std::string_view::npos)
            throw std::invalid_argument("Expected shape or region header");
        const auto header = line.substr(0, colon_offset);
        const auto dimension_separator_offset = header.find('x');
        if (dimension_separator_offset == std::string_view::npos) {
            if (!tree_regions_.empty() ||
                core::parse_integer<std::size_t>(header) != present_shapes_.size())
                throw std::invalid_argument("Shape IDs must be consecutive, starting at zero");
            std::vector<std::string> shape_rows;
            while (line_index < input_lines.size()) {
                const auto input_row = core::trim(input_lines[line_index]);
                if (input_row.empty() || input_row.find(':') != std::string_view::npos)
                    break;
                if (input_row.find_first_not_of(".#") != std::string_view::npos)
                    throw std::invalid_argument("Invalid shape cell");
                shape_rows.emplace_back(input_row);
                ++line_index;
            }
            if (shape_rows.empty())
                throw std::invalid_argument("Missing shape cells");
            auto shape = make_present_shape(shape_rows);
            if (shape.occupied_area == 0)
                throw std::invalid_argument("Shape must occupy at least one cell");
            present_shapes_.push_back(std::move(shape));
        } else {
            const auto width =
                core::parse_integer<int>(header.substr(0, dimension_separator_offset));
            const auto height =
                core::parse_integer<int>(header.substr(dimension_separator_offset + 1));
            if (width <= 0 || height <= 0)
                throw std::invalid_argument("Region dimensions must be positive");
            std::istringstream counts_input(std::string{line.substr(colon_offset + 1)});
            std::vector<int> present_counts;
            for (std::string count_text; counts_input >> count_text;) {
                const auto present_count = core::parse_integer<int>(count_text);
                if (present_count < 0)
                    throw std::invalid_argument("Shape counts must be nonnegative");
                present_counts.push_back(present_count);
            }
            if (present_counts.size() != present_shapes_.size())
                throw std::invalid_argument("Expected one count per shape");
            tree_regions_.push_back({width, height, std::move(present_counts)});
        }
    }
}

// ------------------------------------------------------------
// Shape helpers
// ------------------------------------------------------------

Day12::PresentShape Day12::make_present_shape(const std::vector<std::string>& shape_rows) {
    int height = shape_rows.size();
    int width = 0;
    for (auto& input_row : shape_rows)
        width = std::max(width, static_cast<int>(input_row.size()));

    std::vector<std::vector<bool>> shape_grid(height, std::vector<bool>(width, false));
    for (int row = 0; row < height; ++row)
        for (int column = 0; column < static_cast<int>(shape_rows[row].size()); ++column)
            if (shape_rows[row][column] == '#')
                shape_grid[row][column] = true;

    std::unordered_set<std::string> orientation_keys;
    std::vector<PresentOrientation> orientations;

    auto rotated_grid = shape_grid;
    for (int rotation_index = 0; rotation_index < 4; ++rotation_index) {
        if (rotation_index > 0)
            rotated_grid = rotate_clockwise(rotated_grid);
        for (int reflection_index = 0; reflection_index < 2; ++reflection_index) {
            auto reflected_grid =
                (reflection_index == 0) ? rotated_grid : reflect_horizontally(rotated_grid);
            auto orientation = grid_to_orientation(reflected_grid);
            if (!orientation.cell_offsets.empty()) {
                auto variant_key_text = orientation_key(orientation);
                if (orientation_keys.insert(variant_key_text).second)
                    orientations.push_back(std::move(orientation));
            }
        }
    }

    PresentShape shape;
    shape.orientations = std::move(orientations);
    if (!shape.orientations.empty())
        shape.occupied_area = shape.orientations[0].cell_offsets.size();
    return shape;
}

std::vector<std::vector<bool>> Day12::rotate_clockwise(const std::vector<std::vector<bool>>& grid) {
    int height = grid.size();
    int width = grid[0].size();
    std::vector<std::vector<bool>> result(width, std::vector<bool>(height));
    for (int row = 0; row < height; ++row)
        for (int column = 0; column < width; ++column)
            result[column][height - 1 - row] = grid[row][column];
    return result;
}

std::vector<std::vector<bool>>
Day12::reflect_horizontally(const std::vector<std::vector<bool>>& grid) {
    int height = grid.size();
    int width = grid[0].size();
    std::vector<std::vector<bool>> result(height, std::vector<bool>(width));
    for (int row = 0; row < height; ++row)
        for (int column = 0; column < width; ++column)
            result[row][width - 1 - column] = grid[row][column];
    return result;
}

Day12::PresentOrientation Day12::grid_to_orientation(const std::vector<std::vector<bool>>& grid) {
    int height = grid.size(), width = grid[0].size();
    int first_column = width, first_row = height, last_column = -1, last_row = -1;

    for (int row = 0; row < height; ++row)
        for (int column = 0; column < width; ++column)
            if (grid[row][column]) {
                first_column = std::min(first_column, column);
                first_row = std::min(first_row, row);
                last_column = std::max(last_column, column);
                last_row = std::max(last_row, row);
            }

    if (last_column < first_column)
        return {};

    PresentOrientation orientation;
    orientation.width = last_column - first_column + 1;
    orientation.height = last_row - first_row + 1;

    for (int row = first_row; row <= last_row; ++row)
        for (int column = first_column; column <= last_column; ++column)
            if (grid[row][column])
                orientation.cell_offsets.push_back({column - first_column, row - first_row});

    return orientation;
}

std::string Day12::orientation_key(const PresentOrientation& orientation) {
    std::ostringstream encoded_orientation;
    encoded_orientation << orientation.width << "x" << orientation.height << ":";
    for (auto& offset : orientation.cell_offsets)
        encoded_orientation << offset.column_offset << "," << offset.row_offset << ";";
    return encoded_orientation.str();
}

// ------------------------------------------------------------
// Solver
// ------------------------------------------------------------

std::string Day12::part1() {
    int fitting_region_count = 0;
    for (auto& region : tree_regions_)
        if (presents_fit(region))
            ++fitting_region_count;
    return std::to_string(fitting_region_count);
}

std::string Day12::part2() {
    return "0"; // Day 12 has no second computational puzzle.
}

bool Day12::presents_fit(const TreeRegion& region) const {
    const auto region_area = std::int64_t{region.width} * region.height;
    std::int64_t required_area = 0;
    std::int64_t present_count = 0;
    int slot_width = 0, slot_height = 0;
    for (std::size_t shape_index = 0; shape_index < present_shapes_.size(); ++shape_index) {
        if (region.present_counts[shape_index] == 0)
            continue;
        required_area += std::int64_t{region.present_counts[shape_index]} *
                         present_shapes_[shape_index].occupied_area;
        if (required_area > region_area)
            return false;
        if (!std::ranges::any_of(
                present_shapes_[shape_index].orientations, [&](const auto& orientation) {
                    return orientation.width <= region.width && orientation.height <= region.height;
                }))
            return false;
        present_count += region.present_counts[shape_index];
        const auto& orientation = present_shapes_[shape_index].orientations.front();
        slot_width = std::max(slot_width, orientation.width);
        slot_height = std::max(slot_height, orientation.height);
    }
    if (present_count == 0)
        return true;

    // A disjoint bounding box for every piece is a constructive proof of fit.
    const auto slot_count = std::int64_t{region.width / slot_width} * (region.height / slot_height);
    if (present_count <= slot_count)
        return true;
    return try_pack_region(region);
}

// ------------------------------------------------------------
// Exact packing when area and bounding boxes do not decide the result
// ------------------------------------------------------------

bool Day12::try_pack_region(const TreeRegion& region) const {
    int width = region.width, height = region.height;
    std::vector<std::vector<std::vector<std::size_t>>> placements_by_shape(present_shapes_.size());

    for (std::size_t shape_index = 0; shape_index < present_shapes_.size(); ++shape_index) {
        if (region.present_counts[shape_index] == 0)
            continue;
        for (const auto& orientation : present_shapes_[shape_index].orientations) {
            for (int anchor_row = 0; anchor_row <= height - orientation.height; ++anchor_row)
                for (int anchor_column = 0; anchor_column <= width - orientation.width;
                     ++anchor_column) {
                    std::vector<std::size_t> placement;
                    for (auto& offset : orientation.cell_offsets)
                        placement.push_back(
                            static_cast<std::size_t>(anchor_row + offset.row_offset) * width +
                            anchor_column + offset.column_offset);
                    placements_by_shape[shape_index].push_back(std::move(placement));
                }
        }
    }

    std::vector<bool> occupied_cells(static_cast<std::size_t>(width) * height, false);
    auto remaining_counts = region.present_counts;
    std::vector<std::size_t> first_placement_by_shape(present_shapes_.size(), 0);
    return place_remaining_presents(occupied_cells, remaining_counts, placements_by_shape,
                                    first_placement_by_shape);
}

bool Day12::place_remaining_presents(
    std::vector<bool>& occupied_cells, std::vector<int>& remaining_counts,
    const std::vector<std::vector<std::vector<std::size_t>>>& placements_by_shape,
    std::vector<std::size_t>& first_placement_by_shape) const {
    const auto free_cell_count = std::ranges::count(occupied_cells, false);

    std::int64_t required_area = 0;
    bool all_placed = true;
    for (std::size_t shape_index = 0;
         shape_index < remaining_counts.size() && shape_index < present_shapes_.size();
         ++shape_index) {
        if (remaining_counts[shape_index] > 0) {
            all_placed = false;
            required_area += std::int64_t{remaining_counts[shape_index]} *
                             present_shapes_[shape_index].occupied_area;
        }
    }

    if (all_placed)
        return true;
    if (required_area > free_cell_count)
        return false;

    std::size_t chosen_shape_index = 0, fewest_placements = std::numeric_limits<std::size_t>::max();

    for (std::size_t shape_index = 0; shape_index < remaining_counts.size(); ++shape_index) {
        if (remaining_counts[shape_index] <= 0)
            continue;
        std::size_t feasible_placements = 0;
        for (std::size_t placement_index = first_placement_by_shape[shape_index];
             placement_index < placements_by_shape[shape_index].size(); ++placement_index) {
            const auto& placement = placements_by_shape[shape_index][placement_index];
            if (std::all_of(placement.begin(), placement.end(),
                            [&](std::size_t cell_index) { return !occupied_cells[cell_index]; })) {
                ++feasible_placements;
                if (feasible_placements >= fewest_placements)
                    break;
            }
        }
        if (feasible_placements == 0)
            return false;
        if (feasible_placements < fewest_placements) {
            fewest_placements = feasible_placements;
            chosen_shape_index = shape_index;
        }
    }

    remaining_counts[chosen_shape_index]--;
    const auto first_placement_index = first_placement_by_shape[chosen_shape_index];
    for (std::size_t placement_index = first_placement_index;
         placement_index < placements_by_shape[chosen_shape_index].size(); ++placement_index) {
        const auto& placement = placements_by_shape[chosen_shape_index][placement_index];
        if (std::all_of(placement.begin(), placement.end(),
                        [&](std::size_t cell_index) { return !occupied_cells[cell_index]; })) {
            for (auto cell_index : placement)
                occupied_cells[cell_index] = true;
            first_placement_by_shape[chosen_shape_index] = placement_index + 1;
            if (place_remaining_presents(occupied_cells, remaining_counts, placements_by_shape,
                                         first_placement_by_shape))
                return true;
            for (auto cell_index : placement)
                occupied_cells[cell_index] = false;
        }
    }
    first_placement_by_shape[chosen_shape_index] = first_placement_index;
    remaining_counts[chosen_shape_index]++;
    return false;
}
