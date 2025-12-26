#include "days/day12.h"
#include "core/Registry.h"

#include <sstream>
#include <algorithm>

// 🔌 Registration
static bool registered = [] {
    Registry::instance().register_day(
        12, [] { return std::make_unique<Day12>(); }
    );
    return true;
}();

// ------------------------------------------------------------
// Parsing
// ------------------------------------------------------------

void Day12::set_input(const std::vector<std::string>& lines) {
    shapes.clear();
    regions.clear();

    size_t i = 0;
    while (i < lines.size() && lines[i].find_first_not_of(" \t\r\n") == std::string::npos) {
        ++i;
    }

    // ---------------- Shapes ----------------
    while (i < lines.size()) {
        std::string line = lines[i];
        line.erase(0, line.find_first_not_of(" \t\r\n"));

        if (line.empty()) {
            ++i;
            continue;
        }

        if (is_region_line(line)) {
            break;
        }

        if (!line.empty() && line.back() == ':') {
            ++i;
            std::vector<std::string> rows;
            while (i < lines.size()) {
                std::string s = lines[i];
                s.erase(s.find_last_not_of("\r\n") + 1);
                if (s.find_first_not_of(" \t") == std::string::npos) {
                    ++i;
                    break;
                }
                std::string trimmed = s;
                trimmed.erase(0, trimmed.find_first_not_of(" \t"));
                if (!trimmed.empty() && trimmed.back() == ':') break;
                if (is_region_line(trimmed)) break;
                rows.push_back(trimmed);
                ++i;
            }
            if (!rows.empty()) {
                shapes.push_back(build_shape(rows));
            }
        } else {
            ++i;
        }
    }

    // ---------------- Regions ----------------
    while (i < lines.size()) {
        std::string line = lines[i++];
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        if (line.empty()) continue;
        if (!is_region_line(line)) continue;

        auto pos = line.find(':');
        auto dim = line.substr(0, pos);
        auto cnt = line.substr(pos + 1);

        auto xPos = dim.find('x');
        int w = std::stoi(dim.substr(0, xPos));
        int h = std::stoi(dim.substr(xPos + 1));

        std::istringstream iss(cnt);
        std::vector<int> counts;
        int v;
        while (iss >> v) counts.push_back(v);

        regions.push_back({w, h, counts});
    }
}

// ------------------------------------------------------------
// Shape helpers
// ------------------------------------------------------------

bool Day12::is_region_line(const std::string& s) {
    auto pos = s.find(':');
    if (pos == std::string::npos) return false;
    auto head = s.substr(0, pos);
    auto x = head.find('x');
    if (x == std::string::npos) return false;
    try {
        std::stoi(head.substr(0, x));
        std::stoi(head.substr(x + 1));
        return true;
    } catch (...) {
        return false;
    }
}

Day12::Shape Day12::build_shape(const std::vector<std::string>& rows) {
    int h = rows.size();
    int w = 0;
    for (auto& r : rows) w = std::max(w, (int)r.size());

    std::vector<std::vector<bool>> grid(h, std::vector<bool>(w, false));
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < (int)rows[y].size(); ++x)
            if (rows[y][x] == '#') grid[y][x] = true;

    std::unordered_map<std::string, bool> seen;
    std::vector<Variant> vars;

    auto g = grid;
    for (int r = 0; r < 4; ++r) {
        if (r > 0) g = rotate_grid(g);
        for (int f = 0; f < 2; ++f) {
            auto gf = (f == 0) ? g : flip_grid_h(g);
            auto v = grid_to_variant(gf);
            if (!v.cells.empty()) {
                auto key = variant_key(v);
                if (!seen[key]) {
                    seen[key] = true;
                    vars.push_back(v);
                }
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

    if (maxX < minX) return {};

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
    return "0"; // intentionally stubbed
}

bool Day12::region_can_fit(const Region& r) const {
    int totalArea = 0;
    for (size_t i = 0; i < r.counts.size() && i < shapes.size(); ++i)
        totalArea += r.counts[i] * shapes[i].area;

    if (totalArea > r.width * r.height) return false;

    if (r.width * r.height <= SMALL_BOARD_MAX_AREA)
        return can_tile_region_small(r);

    return true;
}

// ------------------------------------------------------------
// Exact tiling (small boards)
// ------------------------------------------------------------

bool Day12::can_tile_region_small(const Region& r) const {
    int w = r.width, h = r.height;
    std::vector<std::vector<std::vector<int>>> placements(shapes.size());

    for (size_t si = 0; si < shapes.size(); ++si) {
        for (auto& v : shapes[si].variants) {
            for (int y = 0; y <= h - v.height; ++y)
                for (int x = 0; x <= w - v.width; ++x) {
                    std::vector<int> cells;
                    for (auto& c : v.cells)
                        cells.push_back((y + c.y) * w + (x + c.x));
                    placements[si].push_back(cells);
                }
        }
    }

    std::vector<bool> board(w * h, false);
    auto counts = r.counts;
    return bt_tile(board, w, h, counts, placements);
}

bool Day12::bt_tile(
    std::vector<bool>& board,
    int w,
    int h,
    std::vector<int>& counts,
    const std::vector<std::vector<std::vector<int>>>& placements
) const {
    int freeCells = std::count(board.begin(), board.end(), false);

    int needed = 0;
    bool done = true;
    for (size_t i = 0; i < counts.size() && i < shapes.size(); ++i) {
        if (counts[i] > 0) {
            done = false;
            needed += counts[i] * shapes[i].area;
        }
    }

    if (done) return true;
    if (needed > freeCells) return false;

    int best = -1, bestCnt = INT_MAX;

    for (size_t i = 0; i < counts.size(); ++i) {
        if (counts[i] <= 0) continue;
        int feasible = 0;
        for (auto& pl : placements[i]) {
            if (std::all_of(pl.begin(), pl.end(), [&](int idx){ return !board[idx]; })) {
                ++feasible;
                if (feasible >= bestCnt) break;
            }
        }
        if (feasible == 0) return false;
        if (feasible < bestCnt) {
            bestCnt = feasible;
            best = i;
        }
    }

    counts[best]--;
    for (auto& pl : placements[best]) {
        if (std::all_of(pl.begin(), pl.end(), [&](int idx){ return !board[idx]; })) {
            for (int idx : pl) board[idx] = true;
            if (bt_tile(board, w, h, counts, placements)) return true;
            for (int idx : pl) board[idx] = false;
        }
    }
    counts[best]++;
    return false;
}