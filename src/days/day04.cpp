#include "days/day04.h"
#include "core/Registry.h"

#include <array>
#include <queue>

// ------------------------------------------------------------
// Registration (static init)
// ------------------------------------------------------------
static bool registered_day04 = [] {
    Registry::instance().register_day(
        4, [] { return std::make_unique<Day04>(); }
    );
    return true;
}();

// ------------------------------------------------------------
// Direction table (8 neighbors)
// ------------------------------------------------------------
static constexpr std::array<std::pair<int,int>, 8> DIRS {{
    {-1,-1}, {-1,0}, {-1,1},
    { 0,-1},        { 0,1},
    { 1,-1}, { 1,0}, { 1,1}
}};

// ------------------------------------------------------------

void Day04::set_input(const std::vector<std::string>& lines) {
    grid_ = lines;
    rows_ = static_cast<int>(grid_.size());
    cols_ = rows_ ? static_cast<int>(grid_[0].size()) : 0;
}

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

int Day04::count_adjacent(int r, int c) const {
    int count = 0;
    for (auto [dr, dc] : DIRS) {
        int nr = r + dr;
        int nc = c + dc;
        if (nr >= 0 && nr < rows_ &&
            nc >= 0 && nc < cols_ &&
            grid_[nr][nc] == '@') {
            ++count;
        }
    }
    return count;
}

// ------------------------------------------------------------
// Part 1
// ------------------------------------------------------------

std::string Day04::part1() {
    if (rows_ == 0 || cols_ == 0)
        return "0";

    int total = 0;
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            if (grid_[r][c] != '@')
                continue;
            if (count_adjacent(r, c) < 4)
                ++total;
        }
    }
    return std::to_string(total);
}

// ------------------------------------------------------------
// Part 2
// ------------------------------------------------------------

std::string Day04::part2() {
    if (rows_ == 0 || cols_ == 0)
        return "0";

    // on-grid
    std::vector<std::vector<bool>> on(rows_, std::vector<bool>(cols_, false));
    for (int r = 0; r < rows_; ++r)
        for (int c = 0; c < cols_; ++c)
            on[r][c] = (grid_[r][c] == '@');

    // degree grid
    std::vector<std::vector<int>> deg(rows_, std::vector<int>(cols_, 0));
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            if (!on[r][c]) continue;
            for (auto [dr, dc] : DIRS) {
                int nr = r + dr;
                int nc = c + dc;
                if (nr >= 0 && nr < rows_ &&
                    nc >= 0 && nc < cols_ &&
                    on[nr][nc]) {
                    ++deg[r][c];
                }
            }
        }
    }

    struct Cell { int r, c; };
    std::queue<Cell> q;

    for (int r = 0; r < rows_; ++r)
        for (int c = 0; c < cols_; ++c)
            if (on[r][c] && deg[r][c] < 4)
                q.push({r, c});

    int removed = 0;

    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();

        if (!on[r][c])
            continue;

        on[r][c] = false;
        ++removed;

        for (auto [dr, dc] : DIRS) {
            int nr = r + dr;
            int nc = c + dc;
            if (nr < 0 || nr >= rows_ || nc < 0 || nc >= cols_)
                continue;
            if (!on[nr][nc])
                continue;

            if (--deg[nr][nc] == 3)
                q.push({nr, nc});
        }
    }

    return std::to_string(removed);
}