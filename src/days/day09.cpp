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

void Day09::set_input(const std::vector<std::string>& lines) {
    reds.clear();

    for (const auto& line : lines) {
        if (line.empty())
            continue;
        auto comma = line.find(',');
        if (comma == std::string::npos)
            throw std::invalid_argument("Expected a coordinate pair");
        const auto x = core::parse_integer<int>(std::string_view(line).substr(0, comma));
        const auto y = core::parse_integer<int>(std::string_view(line).substr(comma + 1));
        reds.push_back({x, y});
    }
}

// ----------------------------------------------------------
// Part 1
// ----------------------------------------------------------

std::string Day09::part1() {
    return std::to_string(max_area_inclusive(reds));
}

std::int64_t Day09::max_area_inclusive(const std::vector<Pt>& pts) {
    int n = static_cast<int>(pts.size());
    std::int64_t best = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            std::int64_t dx = std::abs(std::int64_t{pts[i].x} - pts[j].x) + 1;
            std::int64_t dy = std::abs(std::int64_t{pts[i].y} - pts[j].y) + 1;
            best = std::max(best, rectangle_area(dx, dy));
        }
    }
    return best;
}

// ----------------------------------------------------------
// Part 2
// ----------------------------------------------------------

std::string Day09::part2() {
    if (reds.size() < 2)
        return "0";

    // Each boundary coordinate and its successor start a distinct interval of
    // integer tiles. Interior gaps can be represented by a single compressed cell.
    std::vector<std::int64_t> xs, ys;
    for (const auto& point : reds) {
        xs.push_back(point.x);
        xs.push_back(std::int64_t{point.x} + 1);
        ys.push_back(point.y);
        ys.push_back(std::int64_t{point.y} + 1);
    }
    const auto compress = [](auto& coordinates) {
        std::ranges::sort(coordinates);
        const auto duplicates = std::ranges::unique(coordinates);
        coordinates.erase(duplicates.begin(), duplicates.end());
    };
    compress(xs);
    compress(ys);

    struct Cell {
        std::size_t x, y;
    };
    std::vector<Cell> vertices;
    for (const auto& point : reds) {
        vertices.push_back(
            {static_cast<std::size_t>(std::ranges::lower_bound(xs, point.x) - xs.begin()),
             static_cast<std::size_t>(std::ranges::lower_bound(ys, point.y) - ys.begin())});
    }
    struct Segment {
        std::size_t left, right, bottom, top;
        bool horizontal;
    };
    std::vector<Segment> segments;
    for (std::size_t i = 0; i < vertices.size(); ++i) {
        const auto a = vertices[i];
        const auto b = vertices[(i + 1) % vertices.size()];
        if (a.x != b.x && a.y != b.y)
            throw std::invalid_argument("Polygon edges must be axis-aligned");
        segments.push_back({std::min(a.x, b.x), std::max(a.x, b.x), std::min(a.y, b.y),
                            std::max(a.y, b.y), a.y == b.y});
    }

    // Scan each compressed row, then build a prefix sum of forbidden cells.
    // A rectangle is valid precisely when its forbidden-cell count is zero.
    const auto stride = xs.size();
    std::vector<std::int64_t> outside(stride * ys.size(), 0);
    std::vector<int> difference(stride);
    std::vector<std::size_t> crossings;
    for (std::size_t y = 0; y + 1 < ys.size(); ++y) {
        std::ranges::fill(difference, 0);
        crossings.clear();
        const auto cover = [&](std::size_t left, std::size_t right) {
            ++difference[left];
            --difference[right + 1];
        };
        for (const auto& edge : segments) {
            if (edge.horizontal) {
                if (y == edge.bottom)
                    cover(edge.left, edge.right);
            } else {
                if (y >= edge.bottom && y <= edge.top)
                    cover(edge.left, edge.left);
                // Half-open vertical edges count each polygon vertex once.
                if (y >= edge.bottom && y < edge.top)
                    crossings.push_back(edge.left);
            }
        }
        std::ranges::sort(crossings);
        if (crossings.size() % 2 != 0)
            throw std::invalid_argument("Invalid polygon boundary");
        for (std::size_t i = 0; i < crossings.size(); i += 2)
            cover(crossings[i], crossings[i + 1]);
        int coverage = 0;
        for (std::size_t x = 0; x + 1 < xs.size(); ++x) {
            coverage += difference[x];
            outside[(y + 1) * stride + x + 1] = (coverage == 0) + outside[y * stride + x + 1] +
                                                outside[(y + 1) * stride + x] -
                                                outside[y * stride + x];
        }
    }

    std::int64_t best = 0;
    for (std::size_t i = 0; i < vertices.size(); ++i) {
        for (std::size_t j = i + 1; j < vertices.size(); ++j) {
            const auto left = std::min(vertices[i].x, vertices[j].x);
            const auto right = std::max(vertices[i].x, vertices[j].x) + 1;
            const auto bottom = std::min(vertices[i].y, vertices[j].y);
            const auto top = std::max(vertices[i].y, vertices[j].y) + 1;
            const auto forbidden = outside[top * stride + right] -
                                   outside[bottom * stride + right] - outside[top * stride + left] +
                                   outside[bottom * stride + left];
            if (forbidden == 0)
                best = std::max(best, rectangle_area(xs[right] - xs[left], ys[top] - ys[bottom]));
        }
    }
    return std::to_string(best);
}
