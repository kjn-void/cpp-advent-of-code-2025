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
const core::DayRegistration<Day04> dayregistration{4};
} // namespace

// ------------------------------------------------------------
// Direction table (8 neighbors)
// ------------------------------------------------------------
static constexpr std::array<std::pair<int, int>, 8> arrayNeighborOffsets{
    {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

// ------------------------------------------------------------

void Day04::SetInput(const std::vector<std::string>& vectorInputLines) {
    if (!vectorInputLines.empty() &&
        !std::ranges::all_of(vectorInputLines, [&](const auto& stringInputRow) {
            return stringInputRow.size() == vectorInputLines.front().size();
        }))
        throw std::invalid_argument("Paper roll grid must be rectangular");
    m_vectorPaperRollDiagram = vectorInputLines;
    m_iRowCount = static_cast<int>(m_vectorPaperRollDiagram.size());
    m_iColumnCount = m_iRowCount ? static_cast<int>(m_vectorPaperRollDiagram[0].size()) : 0;
}

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

int Day04::CountAdjacentRolls(int iRow, int iColumn) const {
    int iAdjacentRollCount = 0;
    for (auto [iRowOffset, iColumnOffset] : arrayNeighborOffsets) {
        int iNeighborRow = iRow + iRowOffset;
        int iNeighborColumn = iColumn + iColumnOffset;
        if (iNeighborRow >= 0 && iNeighborRow < m_iRowCount && iNeighborColumn >= 0 &&
            iNeighborColumn < m_iColumnCount &&
            m_vectorPaperRollDiagram[iNeighborRow][iNeighborColumn] == '@') {
            ++iAdjacentRollCount;
        }
    }
    return iAdjacentRollCount;
}

// ------------------------------------------------------------
// Part 1
// ------------------------------------------------------------

std::string Day04::Part1() {
    if (m_iRowCount == 0 || m_iColumnCount == 0)
        return "0";

    int iAccessibleRollCount = 0;
    for (int iRow = 0; iRow < m_iRowCount; ++iRow) {
        for (int iColumn = 0; iColumn < m_iColumnCount; ++iColumn) {
            if (m_vectorPaperRollDiagram[iRow][iColumn] != '@')
                continue;
            if (CountAdjacentRolls(iRow, iColumn) < 4)
                ++iAccessibleRollCount;
        }
    }
    return std::to_string(iAccessibleRollCount);
}

// ------------------------------------------------------------
// Part 2
// ------------------------------------------------------------

std::string Day04::Part2() {
    if (m_iRowCount == 0 || m_iColumnCount == 0)
        return "0";

    // Rolls still in the diagram
    std::vector<std::vector<bool>> vectorHasRoll(m_iRowCount,
                                                 std::vector<bool>(m_iColumnCount, false));
    for (int iRow = 0; iRow < m_iRowCount; ++iRow)
        for (int iColumn = 0; iColumn < m_iColumnCount; ++iColumn)
            vectorHasRoll[iRow][iColumn] = (m_vectorPaperRollDiagram[iRow][iColumn] == '@');

    // Adjacent roll count for each roll
    std::vector<std::vector<int>> vectorAdjacentRollCounts(m_iRowCount,
                                                           std::vector<int>(m_iColumnCount, 0));
    for (int iRow = 0; iRow < m_iRowCount; ++iRow) {
        for (int iColumn = 0; iColumn < m_iColumnCount; ++iColumn) {
            if (!vectorHasRoll[iRow][iColumn])
                continue;
            for (auto [iRowOffset, iColumnOffset] : arrayNeighborOffsets) {
                int iNeighborRow = iRow + iRowOffset;
                int iNeighborColumn = iColumn + iColumnOffset;
                if (iNeighborRow >= 0 && iNeighborRow < m_iRowCount && iNeighborColumn >= 0 &&
                    iNeighborColumn < m_iColumnCount &&
                    vectorHasRoll[iNeighborRow][iNeighborColumn]) {
                    ++vectorAdjacentRollCounts[iRow][iColumn];
                }
            }
        }
    }

    struct Cell {
        int m_iRow, m_iColumn;
    };
    std::queue<Cell> queueRemovableRolls;

    for (int iRow = 0; iRow < m_iRowCount; ++iRow)
        for (int iColumn = 0; iColumn < m_iColumnCount; ++iColumn)
            if (vectorHasRoll[iRow][iColumn] && vectorAdjacentRollCounts[iRow][iColumn] < 4)
                queueRemovableRolls.push({iRow, iColumn});

    int iRemovedRollCount = 0;

    while (!queueRemovableRolls.empty()) {
        auto [iRow, iColumn] = queueRemovableRolls.front();
        queueRemovableRolls.pop();

        if (!vectorHasRoll[iRow][iColumn])
            continue;

        vectorHasRoll[iRow][iColumn] = false;
        ++iRemovedRollCount;

        for (auto [iRowOffset, iColumnOffset] : arrayNeighborOffsets) {
            int iNeighborRow = iRow + iRowOffset;
            int iNeighborColumn = iColumn + iColumnOffset;
            if (iNeighborRow < 0 || iNeighborRow >= m_iRowCount || iNeighborColumn < 0 ||
                iNeighborColumn >= m_iColumnCount)
                continue;
            if (!vectorHasRoll[iNeighborRow][iNeighborColumn])
                continue;

            if (--vectorAdjacentRollCounts[iNeighborRow][iNeighborColumn] == 3)
                queueRemovableRolls.push({iNeighborRow, iNeighborColumn});
        }
    }

    return std::to_string(iRemovedRollCount);
}
