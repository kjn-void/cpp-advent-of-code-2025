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

int Day04::CrolAdjacent(int rw, int col) const {
    int crolAdjacent = 0;
    for (auto [drwNeighbor, dcolNeighbor] : rgdeltaNeighbors) {
        int rwNeighbor = rw + drwNeighbor;
        int colNeighbor = col + dcolNeighbor;
        if (rwNeighbor >= 0 && rwNeighbor < crw_ && colNeighbor >= 0 && colNeighbor < ccol_ &&
            gridRolls_[rwNeighbor][colNeighbor] == '@') {
            ++crolAdjacent;
        }
    }
    return crolAdjacent;
}

// ------------------------------------------------------------
// Part 1
// ------------------------------------------------------------

std::string Day04::TxtPart1() {
    if (crw_ == 0 || ccol_ == 0)
        return "0";

    int crolAccessible = 0;
    for (int rw = 0; rw < crw_; ++rw) {
        for (int col = 0; col < ccol_; ++col) {
            if (gridRolls_[rw][col] != '@')
                continue;
            if (CrolAdjacent(rw, col) < 4)
                ++crolAccessible;
        }
    }
    return std::to_string(crolAccessible);
}

// ------------------------------------------------------------
// Part 2
// ------------------------------------------------------------

std::string Day04::TxtPart2() {
    if (crw_ == 0 || ccol_ == 0)
        return "0";

    // Rolls still in the grid
    std::vector<std::vector<bool>> gridRollPresent(crw_, std::vector<bool>(ccol_, false));
    for (int rw = 0; rw < crw_; ++rw)
        for (int col = 0; col < ccol_; ++col)
            gridRollPresent[rw][col] = (gridRolls_[rw][col] == '@');

    // Adjacent roll count for each roll
    std::vector<std::vector<int>> gridAdjacentRollCounts(crw_, std::vector<int>(ccol_, 0));
    for (int rw = 0; rw < crw_; ++rw) {
        for (int col = 0; col < ccol_; ++col) {
            if (!gridRollPresent[rw][col])
                continue;
            for (auto [drwNeighbor, dcolNeighbor] : rgdeltaNeighbors) {
                int rwNeighbor = rw + drwNeighbor;
                int colNeighbor = col + dcolNeighbor;
                if (rwNeighbor >= 0 && rwNeighbor < crw_ && colNeighbor >= 0 &&
                    colNeighbor < ccol_ && gridRollPresent[rwNeighbor][colNeighbor]) {
                    ++gridAdjacentRollCounts[rw][col];
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
            if (gridRollPresent[rw][col] && gridAdjacentRollCounts[rw][col] < 4)
                qcelRemovals.push({rw, col});

    int crolRemoved = 0;

    while (!qcelRemovals.empty()) {
        auto [rw, col] = qcelRemovals.front();
        qcelRemovals.pop();

        if (!gridRollPresent[rw][col])
            continue;

        gridRollPresent[rw][col] = false;
        ++crolRemoved;

        for (auto [drwNeighbor, dcolNeighbor] : rgdeltaNeighbors) {
            int rwNeighbor = rw + drwNeighbor;
            int colNeighbor = col + dcolNeighbor;
            if (rwNeighbor < 0 || rwNeighbor >= crw_ || colNeighbor < 0 || colNeighbor >= ccol_)
                continue;
            if (!gridRollPresent[rwNeighbor][colNeighbor])
                continue;

            if (--gridAdjacentRollCounts[rwNeighbor][colNeighbor] == 3)
                qcelRemovals.push({rwNeighbor, colNeighbor});
        }
    }

    return std::to_string(crolRemoved);
}
