#include "days/day07.h"
#include "core/Register.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>

// Registration
namespace {
const core::DayRegistration<Day07> dayregistration{7};
} // namespace

// ------------------------------------------------------------

void Day07::SetInput(const std::vector<std::string>& vectorInputLines) {
    m_vectorManifold = vectorInputLines;
    m_iRowCount = static_cast<int>(m_vectorManifold.size());
    m_iColumnCount = 0;
    m_iStartColumn = -1;
    for (const auto& stringInputRow : m_vectorManifold)
        m_iColumnCount = std::max(m_iColumnCount, static_cast<int>(stringInputRow.size()));
    if (m_iColumnCount == 0)
        return;
    for (auto& stringInputRow : m_vectorManifold)
        stringInputRow.resize(m_iColumnCount, '.');
    const auto uStartColumn = m_vectorManifold.front().find('S');
    if (uStartColumn == std::string::npos ||
        m_vectorManifold.front().find('S', uStartColumn + 1) != std::string::npos)
        throw std::invalid_argument("Tachyon manifold must have one start in its first row");
    m_iStartColumn = static_cast<int>(uStartColumn);
}

// ------------------------------------------------------------
// Part 1 — count splits
// ------------------------------------------------------------

std::string Day07::Part1() {
    if (m_iStartColumn < 0)
        return "0";
    std::vector<bool> vectorBeamBufferA(m_iColumnCount, false);
    std::vector<bool> vectorBeamBufferB(m_iColumnCount, false);

    auto* pvectorActiveBeams = &vectorBeamBufferA;
    auto* pvectorNextBeams = &vectorBeamBufferB;

    (*pvectorActiveBeams)[m_iStartColumn] = true;

    int iSplitCount = 0;

    for (int iRow = 1; iRow < m_iRowCount; ++iRow) {
        std::fill(pvectorNextBeams->begin(), pvectorNextBeams->end(), false);
        const auto& stringInputRow = m_vectorManifold[iRow];

        for (int iColumn = 0; iColumn < m_iColumnCount; ++iColumn) {
            if (!(*pvectorActiveBeams)[iColumn])
                continue;

            if (stringInputRow[iColumn] == '^') {
                iSplitCount++;
                if (iColumn > 0)
                    (*pvectorNextBeams)[iColumn - 1] = true;
                if (iColumn + 1 < m_iColumnCount)
                    (*pvectorNextBeams)[iColumn + 1] = true;
            } else {
                (*pvectorNextBeams)[iColumn] = true;
            }
        }

        std::swap(pvectorActiveBeams, pvectorNextBeams);
    }

    return std::to_string(iSplitCount);
}

// ------------------------------------------------------------
// Part 2 — count timelines
// ------------------------------------------------------------

std::string Day07::Part2() {
    if (m_iStartColumn < 0)
        return "0";
    std::int64_t iExitedTimelines = 0;
    std::vector<std::int64_t> vectorTimelineBufferA(m_iColumnCount, 0);
    std::vector<std::int64_t> vectorTimelineBufferB(m_iColumnCount, 0);

    auto* pvectorActiveTimelines = &vectorTimelineBufferA;
    auto* pvectorNextTimelines = &vectorTimelineBufferB;

    (*pvectorActiveTimelines)[m_iStartColumn] = 1;

    for (int iRow = 1; iRow < m_iRowCount; ++iRow) {
        std::fill(pvectorNextTimelines->begin(), pvectorNextTimelines->end(), 0);
        const auto& stringInputRow = m_vectorManifold[iRow];

        for (int iColumn = 0; iColumn < m_iColumnCount; ++iColumn) {
            std::int64_t iTimelineCount = (*pvectorActiveTimelines)[iColumn];
            if (iTimelineCount == 0)
                continue;

            if (stringInputRow[iColumn] == '^') {
                if (iColumn > 0)
                    (*pvectorNextTimelines)[iColumn - 1] += iTimelineCount;
                else
                    iExitedTimelines += iTimelineCount;
                if (iColumn + 1 < m_iColumnCount)
                    (*pvectorNextTimelines)[iColumn + 1] += iTimelineCount;
                else
                    iExitedTimelines += iTimelineCount;
            } else {
                (*pvectorNextTimelines)[iColumn] += iTimelineCount;
            }
        }

        std::swap(pvectorActiveTimelines, pvectorNextTimelines);
    }

    std::int64_t iTotalTimelines = std::accumulate(pvectorActiveTimelines->begin(),
                                                   pvectorActiveTimelines->end(), std::int64_t{0});

    return std::to_string(iTotalTimelines + iExitedTimelines);
}
