#include "days/day08.h"
#include "core/Register.h"

#include <algorithm>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <tuple>

// Registration
namespace {
const core::DayRegistration<Day08> registration{8};
} // namespace

// -----------------------------------------------------------
// Parsing
// -----------------------------------------------------------

static Day08::Vec3 parse_vec3(const std::string& line) {
    std::stringstream ss(line);
    Day08::Vec3 v{};
    char first_comma{}, second_comma{};
    if (!(ss >> v.x >> first_comma >> v.y >> second_comma >> v.z) || first_comma != ',' ||
        second_comma != ',' || !(ss >> std::ws).eof())
        throw std::invalid_argument("Expected three comma-separated coordinates");
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

std::int64_t Day08::squared_dist(const Vec3& a, const Vec3& b) {
    std::int64_t total = 0;
    const auto add_square = [&](std::int64_t left, std::int64_t right) {
        // Unsigned subtraction also handles differences spanning the signed range.
        const auto distance = left >= right ? std::uint64_t(left) - std::uint64_t(right)
                                            : std::uint64_t(right) - std::uint64_t(left);
        if (distance > 3037000499ULL)
            throw std::overflow_error("Squared distance exceeds int64_t");
        const auto squared = static_cast<std::int64_t>(distance * distance);
        if (squared > std::numeric_limits<std::int64_t>::max() - total)
            throw std::overflow_error("Squared distance exceeds int64_t");
        total += squared;
    };
    add_square(a.x, b.x);
    add_square(a.y, b.y);
    add_square(a.z, b.z);
    return total;
}

std::vector<Day08::Edge> Day08::build_sorted_edges(std::span<const Vec3> pts) {
    const int n = static_cast<int>(pts.size());
    std::vector<Edge> out;
    if (n > 1)
        out.reserve(pts.size() * (pts.size() - 1) / 2);

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            out.push_back({squared_dist(pts[i], pts[j]), i, j});
        }
    }

    std::ranges::sort(out, [](const Edge& a, const Edge& b) {
        return std::tie(a.dist2, a.i, a.j) < std::tie(b.dist2, b.i, b.j);
    });

    return out;
}

// -----------------------------------------------------------
// DSU
// -----------------------------------------------------------

Day08::DSU::DSU(int n) : parent(n), size(n, 1) {
    for (int i = 0; i < n; ++i)
        parent[i] = i;
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
    if (a == b)
        return false;

    if (size[a] < size[b])
        std::swap(a, b);
    parent[b] = a;
    size[a] += size[b];
    return true;
}

// -----------------------------------------------------------
// Core helpers
// -----------------------------------------------------------

std::vector<int> Day08::run_connections(std::span<const Vec3> pts, std::span<const Edge> eds,
                                        int k) {
    if (pts.empty())
        return {};

    DSU uf(static_cast<int>(pts.size()));
    k = std::min(k, static_cast<int>(eds.size()));

    for (int i = 0; i < k; ++i) {
        uf.unite(eds[i].i, eds[i].j);
    }

    std::vector<int> sizes;
    for (int i = 0; i < static_cast<int>(pts.size()); ++i) {
        if (uf.find(i) == i)
            sizes.push_back(uf.size[i]);
    }

    std::ranges::sort(sizes, std::greater<>{});
    return sizes;
}

std::pair<int, int> Day08::run_until_single_circuit(std::span<const Vec3> pts,
                                                    std::span<const Edge> eds) {
    if (pts.size() < 2)
        return {0, 0};

    DSU uf(static_cast<int>(pts.size()));
    int components = static_cast<int>(pts.size());
    int last_i = 0, last_j = 0;

    for (const auto& e : eds) {
        if (uf.unite(e.i, e.j)) {
            --components;
            last_i = e.i;
            last_j = e.j;
            if (components == 1)
                break;
        }
    }

    return {last_i, last_j};
}

// -----------------------------------------------------------
// Parts
// -----------------------------------------------------------

std::string Day08::part1() {
    auto sizes = run_connections(points, edges, 1000);
    if (sizes.size() < 3)
        return "0";

    std::int64_t result = std::int64_t(sizes[0]) * std::int64_t(sizes[1]) * std::int64_t(sizes[2]);

    return std::to_string(result);
}

std::string Day08::part2() {
    if (points.size() < 2)
        return "0";

    auto [i, j] = run_until_single_circuit(points, edges);
    const auto a = points[i].x;
    const auto b = points[j].x;
    const auto magnitude = [](std::int64_t value) {
        const auto bits = static_cast<std::uint64_t>(value);
        return value < 0 ? std::uint64_t{0} - bits : bits;
    };
    const bool negative = (a < 0) != (b < 0);
    const auto limit =
        static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + negative;
    const auto left = magnitude(a), right = magnitude(b);
    if (right != 0 && left > limit / right)
        throw std::overflow_error("Junction coordinate product exceeds int64_t");
    const auto product = left * right;
    if (negative && product == limit)
        return std::to_string(std::numeric_limits<std::int64_t>::min());
    const auto signed_product = static_cast<std::int64_t>(product);
    return std::to_string(negative ? -signed_product : signed_product);
}
