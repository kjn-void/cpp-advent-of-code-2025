#include "days/day04.h"
#include "core/Register.h"

#include <algorithm>
#include <array>
#include <queue>
#include <stdexcept>

// ------------------------------------------------------------
// Registration (static init)
// ------------------------------------------------------------
namespace {
const core::Drg<Day04> drgDay{4};
} // namespace

// ------------------------------------------------------------
// Direction table (8 neighbors)
// ------------------------------------------------------------
static constexpr std::array<std::pair<int, int>, 8> rgdeltaNeighbors{
    {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

// ------------------------------------------------------------

void Day04::SetInput(const std::vector<std::string>& rgusLines) {
    if (!rgusLines.empty() && !std::ranges::all_of(rgusLines, [&](const auto& usRow) {
            return usRow.size() == rgusLines.front().size();
        }))
        throw std::invalid_argument("Paper roll grid must be rectangular");
    gridRolls_ = rgusLines;
    crw_ = static_cast<int>(gridRolls_.size());
    ccol_ = crw_ ? static_cast<int>(gridRolls_[0].size()) : 0;
}

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

int Day04::CntAdjacent(int rw, int col) const {
    int cntAdjacent = 0;
    for (auto [drwNeighbor, dcolNeighbor] : rgdeltaNeighbors) {
        int rwNeighbor = rw + drwNeighbor;
        int colNeighbor = col + dcolNeighbor;
        if (rwNeighbor >= 0 && rwNeighbor < crw_ && colNeighbor >= 0 && colNeighbor < ccol_ &&
            gridRolls_[rwNeighbor][colNeighbor] == '@') {
            ++cntAdjacent;
        }
    }
    return cntAdjacent;
}

// ------------------------------------------------------------
// Part 1
// ------------------------------------------------------------

std::string Day04::TxtPart1() {
    if (crw_ == 0 || ccol_ == 0)
        return "0";

    int cntAccessible = 0;
    for (int rw = 0; rw < crw_; ++rw) {
        for (int col = 0; col < ccol_; ++col) {
            if (gridRolls_[rw][col] != '@')
                continue;
            if (CntAdjacent(rw, col) < 4)
                ++cntAccessible;
        }
    }
    return std::to_string(cntAccessible);
}

// ------------------------------------------------------------
// Part 2
// ------------------------------------------------------------

std::string Day04::TxtPart2() {
    if (crw_ == 0 || ccol_ == 0)
        return "0";

    // on-grid
    std::vector<std::vector<bool>> gridOccupied(crw_, std::vector<bool>(ccol_, false));
    for (int rw = 0; rw < crw_; ++rw)
        for (int col = 0; col < ccol_; ++col)
            gridOccupied[rw][col] = (gridRolls_[rw][col] == '@');

    // degree grid
    std::vector<std::vector<int>> gridDegree(crw_, std::vector<int>(ccol_, 0));
    for (int rw = 0; rw < crw_; ++rw) {
        for (int col = 0; col < ccol_; ++col) {
            if (!gridOccupied[rw][col])
                continue;
            for (auto [drwNeighbor, dcolNeighbor] : rgdeltaNeighbors) {
                int rwNeighbor = rw + drwNeighbor;
                int colNeighbor = col + dcolNeighbor;
                if (rwNeighbor >= 0 && rwNeighbor < crw_ && colNeighbor >= 0 &&
                    colNeighbor < ccol_ && gridOccupied[rwNeighbor][colNeighbor]) {
                    ++gridDegree[rw][col];
                }
            }
        }
    }

    struct Cel {
        int rw, col;
    };
    std::queue<Cel> qcelRemovals;

    for (int rw = 0; rw < crw_; ++rw)
        for (int col = 0; col < ccol_; ++col)
            if (gridOccupied[rw][col] && gridDegree[rw][col] < 4)
                qcelRemovals.push({rw, col});

    int cntRemoved = 0;

    while (!qcelRemovals.empty()) {
        auto [rw, col] = qcelRemovals.front();
        qcelRemovals.pop();

        if (!gridOccupied[rw][col])
            continue;

        gridOccupied[rw][col] = false;
        ++cntRemoved;

        for (auto [drwNeighbor, dcolNeighbor] : rgdeltaNeighbors) {
            int rwNeighbor = rw + drwNeighbor;
            int colNeighbor = col + dcolNeighbor;
            if (rwNeighbor < 0 || rwNeighbor >= crw_ || colNeighbor < 0 || colNeighbor >= ccol_)
                continue;
            if (!gridOccupied[rwNeighbor][colNeighbor])
                continue;

            if (--gridDegree[rwNeighbor][colNeighbor] == 3)
                qcelRemovals.push({rwNeighbor, colNeighbor});
        }
    }

    return std::to_string(cntRemoved);
}
