#include "days/day08.h"
#include "core/Registry.h"

#include <algorithm>
#include <sstream>

// 🔌 Static registration
static bool registered = [] {
    Registry::instance().register_day(
        8, [] { return std::make_unique<Day08>(); }
    );
    return true;
}();

// -----------------------------------------------------------
// Parsing
// -----------------------------------------------------------

static Day08::Vec3 parse_vec3(const std::string& line) {
    std::stringstream ss(line);
    Day08::Vec3 v{};
    char comma;
    ss >> v.x >> comma >> v.y >> comma >> v.z;
    return v;
}

void Day08::set_input(const std::vector<std::string>& lines) {
    points.clear();

    for (const auto& ln : lines) {
        if (!ln.empty()) {
            points.push_back(parse_vec3(ln));
        }
    }

    edges = build_sorted_edges(points);
}

// -----------------------------------------------------------
// Distance & Edge Preparation
// -----------------------------------------------------------

int64_t Day08::squared_dist(const Vec3& a, const Vec3& b) {
    int64_t dx = a.x - b.x;
    int64_t dy = a.y - b.y;
    int64_t dz = a.z - b.z;
    return dx*dx + dy*dy + dz*dz;
}

std::vector<Day08::Edge>
Day08::build_sorted_edges(const std::vector<Vec3>& pts) {
    const int n = static_cast<int>(pts.size());
    std::vector<Edge> out;
    out.reserve(n * (n - 1) / 2);

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            out.push_back({
                squared_dist(pts[i], pts[j]),
                i, j
            });
        }
    }

    std::sort(out.begin(), out.end(),
        [](const Edge& a, const Edge& b) {
            return a.dist2 < b.dist2;
        });

    return out;
}

// -----------------------------------------------------------
// DSU
// -----------------------------------------------------------

Day08::DSU::DSU(int n) : parent(n), size(n, 1) {
    for (int i = 0; i < n; ++i) parent[i] = i;
}

int Day08::DSU::find(int x) {
    while (parent[x] != x) {
        parent[x] = parent[parent[x]];
        x = parent[x];
    }
    return x;
}

bool Day08::DSU::unite(int a, int b) {
    a = find(a);
    b = find(b);
    if (a == b) return false;

    if (size[a] < size[b]) std::swap(a, b);
    parent[b] = a;
    size[a] += size[b];
    return true;
}

// -----------------------------------------------------------
// Core helpers
// -----------------------------------------------------------

std::vector<int> Day08::run_connections(
    const std::vector<Vec3>& pts,
    const std::vector<Edge>& eds,
    int k
) {
    if (pts.empty()) return {};

    DSU uf(static_cast<int>(pts.size()));
    k = std::min(k, static_cast<int>(eds.size()));

    for (int i = 0; i < k; ++i) {
        uf.unite(eds[i].i, eds[i].j);
    }

    std::unordered_map<int,int> comps;
    for (int i = 0; i < (int)pts.size(); ++i) {
        int r = uf.find(i);
        comps[r] = uf.size[r];
    }

    std::vector<int> sizes;
    for (auto& [_, sz] : comps) {
        sizes.push_back(sz);
    }

    std::sort(sizes.begin(), sizes.end(), std::greater<>());
    return sizes;
}

std::pair<int,int> Day08::run_until_single_circuit(
    const std::vector<Vec3>& pts,
    const std::vector<Edge>& eds
) {
    if (pts.size() < 2) return {0, 0};

    DSU uf(static_cast<int>(pts.size()));
    int components = static_cast<int>(pts.size());
    int last_i = 0, last_j = 0;

    for (const auto& e : eds) {
        if (uf.unite(e.i, e.j)) {
            --components;
            last_i = e.i;
            last_j = e.j;
            if (components == 1) break;
        }
    }

    return {last_i, last_j};
}

// -----------------------------------------------------------
// Parts
// -----------------------------------------------------------

std::string Day08::part1() {
    auto sizes = run_connections(points, edges, 1000);
    if (sizes.size() < 3) return "0";

    int64_t result =
        int64_t(sizes[0]) *
        int64_t(sizes[1]) *
        int64_t(sizes[2]);

    return std::to_string(result);
}

std::string Day08::part2() {
    if (points.size() < 2) return "0";

    auto [i, j] = run_until_single_circuit(points, edges);
    return std::to_string(points[i].x * points[j].x);
}