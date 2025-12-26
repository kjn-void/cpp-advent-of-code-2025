#include "days/day09.h"
#include "core/Registry.h"

#include <sstream>
#include <cstdlib>

using namespace std;

// 🔌 Registration
static bool registered = [] {
    Registry::instance().register_day(
        9, [] { return make_unique<Day09>(); }
    );
    return true;
}();

// ----------------------------------------------------------
// Input
// ----------------------------------------------------------

void Day09::set_input(const vector<string>& lines) {
    reds.clear();
    edges.clear();

    for (const auto& line : lines) {
        if (line.empty()) continue;
        auto comma = line.find(',');
        int x = stoi(line.substr(0, comma));
        int y = stoi(line.substr(comma + 1));
        reds.push_back({x, y});
    }
}

// ----------------------------------------------------------
// Part 1
// ----------------------------------------------------------

string Day09::part1() {
    return to_string(max_area_inclusive(reds));
}

int Day09::max_area_inclusive(const vector<Pt>& pts) {
    int n = (int)pts.size();
    int best = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int dx = abs_int(pts[i].x - pts[j].x) + 1;
            int dy = abs_int(pts[i].y - pts[j].y) + 1;
            best = max(best, dx * dy);
        }
    }
    return best;
}

// ----------------------------------------------------------
// Part 2
// ----------------------------------------------------------

string Day09::part2() {
    if (reds.size() < 2) return "0";
    if (edges.empty()) build_edges();

    int best = 0;
    int n = (int)reds.size();

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            auto a = reds[i];
            auto b = reds[j];

            int x1 = min_int(a.x, b.x);
            int x2 = max_int(a.x, b.x);
            int y1 = min_int(a.y, b.y);
            int y2 = max_int(a.y, b.y);

            int area = (x2 - x1 + 1) * (y2 - y1 + 1);
            if (area <= best) continue;

            Pt c3{x1, y2};
            Pt c4{x2, y1};

            if (!point_inside_or_on(c3) || !point_inside_or_on(c4)) continue;
            if (rectangle_cut_by_polygon(x1, y1, x2, y2)) continue;

            best = area;
        }
    }

    return to_string(best);
}

// ----------------------------------------------------------
// Polygon edges
// ----------------------------------------------------------

void Day09::build_edges() {
    int n = (int)reds.size();
    edges.clear();
    edges.reserve(n);

    for (int i = 0; i < n; ++i) {
        auto a = reds[i];
        auto b = reds[(i + 1) % n];

        Edge e{a.x, a.y, b.x, b.y, false};
        if (a.y == b.y) {
            e.hor = true;
            if (e.x1 > e.x2) swap(e.x1, e.x2);
        } else {
            if (e.y1 > e.y2) swap(e.y1, e.y2);
        }
        edges.push_back(e);
    }
}

// ----------------------------------------------------------
// Point in polygon
// ----------------------------------------------------------

bool Day09::point_inside_or_on(const Pt& p) const {
    for (const auto& e : edges) {
        if (e.hor) {
            if (p.y == e.y1 && p.x >= e.x1 && p.x <= e.x2)
                return true;
        } else {
            if (p.x == e.x1 && p.y >= e.y1 && p.y <= e.y2)
                return true;
        }
    }
    return point_in_polygon_ray_cast(p, reds);
}

bool Day09::point_in_polygon_ray_cast(const Pt& p, const vector<Pt>& poly) {
    bool inside = false;
    int n = (int)poly.size();
    int j = n - 1;

    for (int i = 0; i < n; ++i) {
        const auto& pi = poly[i];
        const auto& pj = poly[j];

        if ((pi.y > p.y) != (pj.y > p.y)) {
            double xint = pj.x + double(p.y - pj.y) *
                          double(pi.x - pj.x) /
                          double(pi.y - pj.y);
            if (p.x < xint) inside = !inside;
        }
        j = i;
    }
    return inside;
}

// ----------------------------------------------------------
// Rectangle cut test
// ----------------------------------------------------------

bool Day09::rectangle_cut_by_polygon(int x1, int y1, int x2, int y2) const {
    if (x1 == x2 || y1 == y2) return false;

    for (const auto& e : edges) {
        if (e.hor) {
            int y = e.y1;
            if (y <= y1 || y >= y2) continue;
            if (max_int(e.x1, x1) < min_int(e.x2, x2)) return true;
        } else {
            int x = e.x1;
            if (x <= x1 || x >= x2) continue;
            if (max_int(e.y1, y1) < min_int(e.y2, y2)) return true;
        }
    }
    return false;
}