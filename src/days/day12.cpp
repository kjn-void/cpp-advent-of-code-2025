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

void Day12::set_input(const std::vector<std::string>& lines) {
    shapes.clear();
    regions.clear();

    for (std::size_t i = 0; i < lines.size();) {
        const auto line = core::trim(lines[i++]);
        if (line.empty())
            continue;
        const auto colon = line.find(':');
        if (colon == std::string_view::npos)
            throw std::invalid_argument("Expected shape or region header");
        const auto header = line.substr(0, colon);
        const auto x = header.find('x');
        if (x == std::string_view::npos) {
            if (!regions.empty() || core::parse_integer<std::size_t>(header) != shapes.size())
                throw std::invalid_argument("Shape IDs must be consecutive, starting at zero");
            std::vector<std::string> rows;
            while (i < lines.size()) {
                const auto row = core::trim(lines[i]);
                if (row.empty() || row.find(':') != std::string_view::npos)
                    break;
                if (row.find_first_not_of(".#") != std::string_view::npos)
                    throw std::invalid_argument("Invalid shape cell");
                rows.emplace_back(row);
                ++i;
            }
            if (rows.empty())
                throw std::invalid_argument("Missing shape cells");
            auto shape = build_shape(rows);
            if (shape.area == 0)
                throw std::invalid_argument("Shape must occupy at least one cell");
            shapes.push_back(std::move(shape));
        } else {
            const auto width = core::parse_integer<int>(header.substr(0, x));
            const auto height = core::parse_integer<int>(header.substr(x + 1));
            if (width <= 0 || height <= 0)
                throw std::invalid_argument("Region dimensions must be positive");
            std::istringstream input(std::string{line.substr(colon + 1)});
            std::vector<int> counts;
            for (std::string token; input >> token;) {
                const auto count = core::parse_integer<int>(token);
                if (count < 0)
                    throw std::invalid_argument("Shape counts must be nonnegative");
                counts.push_back(count);
            }
            if (counts.size() != shapes.size())
                throw std::invalid_argument("Expected one count per shape");
            regions.push_back({width, height, std::move(counts)});
        }
    }
}

// ------------------------------------------------------------
// Shape helpers
// ------------------------------------------------------------

Day12::Shape Day12::build_shape(const std::vector<std::string>& rows) {
    int h = rows.size();
    int w = 0;
    for (auto& r : rows)
        w = std::max(w, static_cast<int>(r.size()));

    std::vector<std::vector<bool>> grid(h, std::vector<bool>(w, false));
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < static_cast<int>(rows[y].size()); ++x)
            if (rows[y][x] == '#')
                grid[y][x] = true;

    std::unordered_set<std::string> seen;
    std::vector<Variant> vars;

    auto g = grid;
    for (int r = 0; r < 4; ++r) {
        if (r > 0)
            g = rotate_grid(g);
        for (int f = 0; f < 2; ++f) {
            auto gf = (f == 0) ? g : flip_grid_h(g);
            auto v = grid_to_variant(gf);
            if (!v.cells.empty()) {
                auto key = variant_key(v);
                if (seen.insert(key).second)
                    vars.push_back(std::move(v));
            }
        }
    }

    Shape s;
    s.variants = std::move(vars);
    if (!s.variants.empty())
        s.area = s.variants[0].cells.size();
    return s;
}

std::vector<std::vector<bool>> Day12::rotate_grid(const std::vector<std::vector<bool>>& g) {
    int h = g.size();
    int w = g[0].size();
    std::vector<std::vector<bool>> r(w, std::vector<bool>(h));
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            r[x][h - 1 - y] = g[y][x];
    return r;
}

std::vector<std::vector<bool>> Day12::flip_grid_h(const std::vector<std::vector<bool>>& g) {
    int h = g.size();
    int w = g[0].size();
    std::vector<std::vector<bool>> r(h, std::vector<bool>(w));
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            r[y][w - 1 - x] = g[y][x];
    return r;
}

Day12::Variant Day12::grid_to_variant(const std::vector<std::vector<bool>>& g) {
    int h = g.size(), w = g[0].size();
    int minX = w, minY = h, maxX = -1, maxY = -1;

    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            if (g[y][x]) {
                minX = std::min(minX, x);
                minY = std::min(minY, y);
                maxX = std::max(maxX, x);
                maxY = std::max(maxY, y);
            }

    if (maxX < minX)
        return {};

    Variant v;
    v.width = maxX - minX + 1;
    v.height = maxY - minY + 1;

    for (int y = minY; y <= maxY; ++y)
        for (int x = minX; x <= maxX; ++x)
            if (g[y][x])
                v.cells.push_back({x - minX, y - minY});

    return v;
}

std::string Day12::variant_key(const Variant& v) {
    std::ostringstream oss;
    oss << v.width << "x" << v.height << ":";
    for (auto& c : v.cells)
        oss << c.x << "," << c.y << ";";
    return oss.str();
}

// ------------------------------------------------------------
// Solver
// ------------------------------------------------------------

std::string Day12::part1() {
    int ok = 0;
    for (auto& r : regions)
        if (region_can_fit(r))
            ++ok;
    return std::to_string(ok);
}

std::string Day12::part2() {
    return "0"; // Day 12 has no second computational puzzle.
}

bool Day12::region_can_fit(const Region& r) const {
    const auto board_area = std::int64_t{r.width} * r.height;
    std::int64_t total_area = 0;
    std::int64_t pieces = 0;
    int slot_width = 0, slot_height = 0;
    for (std::size_t i = 0; i < shapes.size(); ++i) {
        if (r.counts[i] == 0)
            continue;
        total_area += std::int64_t{r.counts[i]} * shapes[i].area;
        if (total_area > board_area)
            return false;
        if (!std::ranges::any_of(shapes[i].variants, [&](const auto& variant) {
                return variant.width <= r.width && variant.height <= r.height;
            }))
            return false;
        pieces += r.counts[i];
        const auto& variant = shapes[i].variants.front();
        slot_width = std::max(slot_width, variant.width);
        slot_height = std::max(slot_height, variant.height);
    }
    if (pieces == 0)
        return true;

    // A disjoint bounding box for every piece is a constructive proof of fit.
    const auto slots = std::int64_t{r.width / slot_width} * (r.height / slot_height);
    if (pieces <= slots)
        return true;
    return can_pack_region(r);
}

// ------------------------------------------------------------
// Exact packing when area and bounding boxes do not decide the result
// ------------------------------------------------------------

bool Day12::can_pack_region(const Region& r) const {
    int w = r.width, h = r.height;
    std::vector<std::vector<std::vector<std::size_t>>> placements(shapes.size());

    for (std::size_t si = 0; si < shapes.size(); ++si) {
        if (r.counts[si] == 0)
            continue;
        for (const auto& v : shapes[si].variants) {
            for (int y = 0; y <= h - v.height; ++y)
                for (int x = 0; x <= w - v.width; ++x) {
                    std::vector<std::size_t> cells;
                    for (auto& c : v.cells)
                        cells.push_back(static_cast<std::size_t>(y + c.y) * w + x + c.x);
                    placements[si].push_back(std::move(cells));
                }
        }
    }

    std::vector<bool> board(static_cast<std::size_t>(w) * h, false);
    auto counts = r.counts;
    std::vector<std::size_t> first_placement(shapes.size(), 0);
    return pack(board, counts, placements, first_placement);
}

bool Day12::pack(std::vector<bool>& board, std::vector<int>& counts,
                 const std::vector<std::vector<std::vector<std::size_t>>>& placements,
                 std::vector<std::size_t>& first_placement) const {
    const auto freeCells = std::ranges::count(board, false);

    std::int64_t needed = 0;
    bool done = true;
    for (std::size_t i = 0; i < counts.size() && i < shapes.size(); ++i) {
        if (counts[i] > 0) {
            done = false;
            needed += std::int64_t{counts[i]} * shapes[i].area;
        }
    }

    if (done)
        return true;
    if (needed > freeCells)
        return false;

    std::size_t best = 0, bestCnt = std::numeric_limits<std::size_t>::max();

    for (std::size_t i = 0; i < counts.size(); ++i) {
        if (counts[i] <= 0)
            continue;
        std::size_t feasible = 0;
        for (std::size_t index = first_placement[i]; index < placements[i].size(); ++index) {
            const auto& pl = placements[i][index];
            if (std::all_of(pl.begin(), pl.end(), [&](std::size_t idx) { return !board[idx]; })) {
                ++feasible;
                if (feasible >= bestCnt)
                    break;
            }
        }
        if (feasible == 0)
            return false;
        if (feasible < bestCnt) {
            bestCnt = feasible;
            best = i;
        }
    }

    counts[best]--;
    const auto first = first_placement[best];
    for (std::size_t index = first; index < placements[best].size(); ++index) {
        const auto& pl = placements[best][index];
        if (std::all_of(pl.begin(), pl.end(), [&](std::size_t idx) { return !board[idx]; })) {
            for (auto idx : pl)
                board[idx] = true;
            first_placement[best] = index + 1;
            if (pack(board, counts, placements, first_placement))
                return true;
            for (auto idx : pl)
                board[idx] = false;
        }
    }
    first_placement[best] = first;
    counts[best]++;
    return false;
}
