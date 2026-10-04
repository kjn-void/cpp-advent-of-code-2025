#include "days/day09.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <stdexcept>

// Registration
namespace {
const core::DayRegistration<Day09> registration{9};

std::int64_t rectangle_area(std::int64_t width, std::int64_t height) {
    if (width > std::numeric_limits<std::int64_t>::max() / height)
        throw std::overflow_error("Rectangle area exceeds int64_t");
    return width * height;
}
} // namespace

// ----------------------------------------------------------
// Input
// ----------------------------------------------------------

void Day09::set_input(const std::vector<std::string>& input_lines) {
    red_tiles_.clear();

    for (const auto& line : input_lines) {
        if (line.empty())
            continue;
        auto comma_offset = line.find(',');
        if (comma_offset == std::string::npos)
            throw std::invalid_argument("Expected a coordinate pair");
        const auto x = core::parse_integer<int>(std::string_view(line).substr(0, comma_offset));
        const auto y = core::parse_integer<int>(std::string_view(line).substr(comma_offset + 1));
        red_tiles_.push_back({x, y});
    }
}

// ----------------------------------------------------------
// Part 1
// ----------------------------------------------------------

std::string Day09::part1() {
    return std::to_string(largest_rectangle_area(red_tiles_));
}

std::int64_t Day09::largest_rectangle_area(const std::vector<Tile>& red_tiles) {
    int red_tile_count = static_cast<int>(red_tiles.size());
    std::int64_t largest_area = 0;

    for (int first_tile_index = 0; first_tile_index < red_tile_count; ++first_tile_index) {
        for (int second_tile_index = first_tile_index + 1; second_tile_index < red_tile_count;
             ++second_tile_index) {
            std::int64_t width = std::abs(std::int64_t{red_tiles[first_tile_index].x} -
                                          red_tiles[second_tile_index].x) +
                                 1;
            std::int64_t height = std::abs(std::int64_t{red_tiles[first_tile_index].y} -
                                           red_tiles[second_tile_index].y) +
                                  1;
            largest_area = std::max(largest_area, rectangle_area(width, height));
        }
    }
    return largest_area;
}

// ----------------------------------------------------------
// Part 2
// ----------------------------------------------------------

std::string Day09::part2() {
    if (red_tiles_.size() < 2)
        return "0";

    // Each boundary coordinate and its successor start a distinct interval of
    // integer tiles. Interior gaps can be represented by a single compressed cell.
    std::vector<std::int64_t> x_boundaries, y_boundaries;
    for (const auto& tile : red_tiles_) {
        x_boundaries.push_back(tile.x);
        x_boundaries.push_back(std::int64_t{tile.x} + 1);
        y_boundaries.push_back(tile.y);
        y_boundaries.push_back(std::int64_t{tile.y} + 1);
    }
    const auto sort_unique_coordinates = [](auto& coordinates) {
        std::ranges::sort(coordinates);
        const auto duplicates = std::ranges::unique(coordinates);
        coordinates.erase(duplicates.begin(), duplicates.end());
    };
    sort_unique_coordinates(x_boundaries);
    sort_unique_coordinates(y_boundaries);

    struct CompressedCell {
        std::size_t column, row;
    };
    std::vector<CompressedCell> compressed_red_tiles;
    for (const auto& tile : red_tiles_) {
        compressed_red_tiles.push_back(
            {static_cast<std::size_t>(std::ranges::lower_bound(x_boundaries, tile.x) -
                                      x_boundaries.begin()),
             static_cast<std::size_t>(std::ranges::lower_bound(y_boundaries, tile.y) -
                                      y_boundaries.begin())});
    }
    struct BoundarySegment {
        std::size_t first_column, last_column, first_row, last_row;
        bool is_horizontal;
    };
    std::vector<BoundarySegment> boundary_segments;
    for (std::size_t tile_index = 0; tile_index < compressed_red_tiles.size(); ++tile_index) {
        const auto tile = compressed_red_tiles[tile_index];
        const auto next_tile = compressed_red_tiles[(tile_index + 1) % compressed_red_tiles.size()];
        if (tile.column != next_tile.column && tile.row != next_tile.row)
            throw std::invalid_argument("Polygon edges must be axis-aligned");
        boundary_segments.push_back({std::min(tile.column, next_tile.column),
                                     std::max(tile.column, next_tile.column),
                                     std::min(tile.row, next_tile.row),
                                     std::max(tile.row, next_tile.row), tile.row == next_tile.row});
    }

    // Scan each compressed row, then build a prefix sum of forbidden cells: those
    // that are neither red nor green. A rectangle is valid precisely when its
    // forbidden-cell count is zero.
    const auto prefix_stride = x_boundaries.size();
    std::vector<std::int64_t> forbidden_prefix_sum(prefix_stride * y_boundaries.size(), 0);
    std::vector<int> coverage_deltas(prefix_stride);
    std::vector<std::size_t> crossing_columns;
    for (std::size_t row = 0; row + 1 < y_boundaries.size(); ++row) {
        std::ranges::fill(coverage_deltas, 0);
        crossing_columns.clear();
        const auto cover_columns = [&](std::size_t first_column, std::size_t last_column) {
            ++coverage_deltas[first_column];
            --coverage_deltas[last_column + 1];
        };
        for (const auto& segment : boundary_segments) {
            if (segment.is_horizontal) {
                if (row == segment.first_row)
                    cover_columns(segment.first_column, segment.last_column);
            } else {
                if (row >= segment.first_row && row <= segment.last_row)
                    cover_columns(segment.first_column, segment.first_column);
                // Half-open vertical edges count each polygon vertex once.
                if (row >= segment.first_row && row < segment.last_row)
                    crossing_columns.push_back(segment.first_column);
            }
        }
        std::ranges::sort(crossing_columns);
        if (crossing_columns.size() % 2 != 0)
            throw std::invalid_argument("Invalid polygon boundary");
        for (std::size_t crossing_index = 0; crossing_index < crossing_columns.size();
             crossing_index += 2)
            cover_columns(crossing_columns[crossing_index], crossing_columns[crossing_index + 1]);
        int red_green_coverage = 0;
        for (std::size_t column = 0; column + 1 < x_boundaries.size(); ++column) {
            red_green_coverage += coverage_deltas[column];
            forbidden_prefix_sum[(row + 1) * prefix_stride + column + 1] =
                (red_green_coverage == 0) + forbidden_prefix_sum[row * prefix_stride + column + 1] +
                forbidden_prefix_sum[(row + 1) * prefix_stride + column] -
                forbidden_prefix_sum[row * prefix_stride + column];
        }
    }

    std::int64_t largest_area = 0;
    for (std::size_t first_tile_index = 0; first_tile_index < compressed_red_tiles.size();
         ++first_tile_index) {
        for (std::size_t second_tile_index = first_tile_index + 1;
             second_tile_index < compressed_red_tiles.size(); ++second_tile_index) {
            const auto first_column = std::min(compressed_red_tiles[first_tile_index].column,
                                               compressed_red_tiles[second_tile_index].column);
            const auto column_end = std::max(compressed_red_tiles[first_tile_index].column,
                                             compressed_red_tiles[second_tile_index].column) +
                                    1;
            const auto first_row = std::min(compressed_red_tiles[first_tile_index].row,
                                            compressed_red_tiles[second_tile_index].row);
            const auto row_end = std::max(compressed_red_tiles[first_tile_index].row,
                                          compressed_red_tiles[second_tile_index].row) +
                                 1;
            const auto forbidden_cell_count =
                forbidden_prefix_sum[row_end * prefix_stride + column_end] -
                forbidden_prefix_sum[first_row * prefix_stride + column_end] -
                forbidden_prefix_sum[row_end * prefix_stride + first_column] +
                forbidden_prefix_sum[first_row * prefix_stride + first_column];
            if (forbidden_cell_count == 0)
                largest_area =
                    std::max(largest_area,
                             rectangle_area(x_boundaries[column_end] - x_boundaries[first_column],
                                            y_boundaries[row_end] - y_boundaries[first_row]));
        }
    }
    return std::to_string(largest_area);
}
