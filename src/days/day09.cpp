#include "days/day09.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <stdexcept>

// Registration
namespace {
const core::DayRegistration<Day09> dayregistration{9};

std::int64_t RectangleArea(std::int64_t iWidth, std::int64_t iHeight) {
    if (iWidth > std::numeric_limits<std::int64_t>::max() / iHeight)
        throw std::overflow_error("Rectangle area exceeds int64_t");
    return iWidth * iHeight;
}
} // namespace

// ----------------------------------------------------------
// Input
// ----------------------------------------------------------

void Day09::SetInput(const std::vector<std::string>& vectorInputLines) {
    m_vectorRedTiles.clear();

    for (const auto& stringLine : vectorInputLines) {
        if (stringLine.empty())
            continue;
        auto uCommaOffset = stringLine.find(',');
        if (uCommaOffset == std::string::npos)
            throw std::invalid_argument("Expected a coordinate pair");
        const auto iX =
            core::ParseInteger<int>(std::string_view(stringLine).substr(0, uCommaOffset));
        const auto iY =
            core::ParseInteger<int>(std::string_view(stringLine).substr(uCommaOffset + 1));
        m_vectorRedTiles.push_back({iX, iY});
    }
}

// ----------------------------------------------------------
// Part 1
// ----------------------------------------------------------

std::string Day09::Part1() {
    return std::to_string(LargestRectangleArea(m_vectorRedTiles));
}

std::int64_t Day09::LargestRectangleArea(const std::vector<Tile>& vectorRedTiles) {
    int iRedTileCount = static_cast<int>(vectorRedTiles.size());
    std::int64_t iLargestArea = 0;

    for (int iFirstTileIndex = 0; iFirstTileIndex < iRedTileCount; ++iFirstTileIndex) {
        for (int iSecondTileIndex = iFirstTileIndex + 1; iSecondTileIndex < iRedTileCount;
             ++iSecondTileIndex) {
            std::int64_t iWidth = std::abs(std::int64_t{vectorRedTiles[iFirstTileIndex].m_iX} -
                                           vectorRedTiles[iSecondTileIndex].m_iX) +
                                  1;
            std::int64_t iHeight = std::abs(std::int64_t{vectorRedTiles[iFirstTileIndex].m_iY} -
                                            vectorRedTiles[iSecondTileIndex].m_iY) +
                                   1;
            iLargestArea = std::max(iLargestArea, RectangleArea(iWidth, iHeight));
        }
    }
    return iLargestArea;
}

// ----------------------------------------------------------
// Part 2
// ----------------------------------------------------------

std::string Day09::Part2() {
    if (m_vectorRedTiles.size() < 2)
        return "0";

    // Each boundary coordinate and its successor start a distinct interval of
    // integer tiles. Interior gaps can be represented by a single compressed cell.
    std::vector<std::int64_t> vectorXBoundaries, vectorYBoundaries;
    for (const auto& tile : m_vectorRedTiles) {
        vectorXBoundaries.push_back(tile.m_iX);
        vectorXBoundaries.push_back(std::int64_t{tile.m_iX} + 1);
        vectorYBoundaries.push_back(tile.m_iY);
        vectorYBoundaries.push_back(std::int64_t{tile.m_iY} + 1);
    }
    const auto sort_unique_coordinates_ = [](auto& vectorCoordinates) {
        std::ranges::sort(vectorCoordinates);
        const auto subrangeDuplicates = std::ranges::unique(vectorCoordinates);
        vectorCoordinates.erase(subrangeDuplicates.begin(), subrangeDuplicates.end());
    };
    sort_unique_coordinates_(vectorXBoundaries);
    sort_unique_coordinates_(vectorYBoundaries);

    struct CompressedCell {
        std::size_t m_uColumn, m_uRow;
    };
    std::vector<CompressedCell> vectorCompressedRedTiles;
    for (const auto& tile : m_vectorRedTiles) {
        vectorCompressedRedTiles.push_back(
            {static_cast<std::size_t>(std::ranges::lower_bound(vectorXBoundaries, tile.m_iX) -
                                      vectorXBoundaries.begin()),
             static_cast<std::size_t>(std::ranges::lower_bound(vectorYBoundaries, tile.m_iY) -
                                      vectorYBoundaries.begin())});
    }
    struct BoundarySegment {
        std::size_t m_uFirstColumn, m_uLastColumn, m_uFirstRow, m_uLastRow;
        bool m_bIsHorizontal;
    };
    std::vector<BoundarySegment> vectorBoundarySegments;
    for (std::size_t uTileIndex = 0; uTileIndex < vectorCompressedRedTiles.size(); ++uTileIndex) {
        const auto compressedcellTile = vectorCompressedRedTiles[uTileIndex];
        const auto compressedcellNextTile =
            vectorCompressedRedTiles[(uTileIndex + 1) % vectorCompressedRedTiles.size()];
        if (compressedcellTile.m_uColumn != compressedcellNextTile.m_uColumn &&
            compressedcellTile.m_uRow != compressedcellNextTile.m_uRow)
            throw std::invalid_argument("Polygon edges must be axis-aligned");
        vectorBoundarySegments.push_back(
            {std::min(compressedcellTile.m_uColumn, compressedcellNextTile.m_uColumn),
             std::max(compressedcellTile.m_uColumn, compressedcellNextTile.m_uColumn),
             std::min(compressedcellTile.m_uRow, compressedcellNextTile.m_uRow),
             std::max(compressedcellTile.m_uRow, compressedcellNextTile.m_uRow),
             compressedcellTile.m_uRow == compressedcellNextTile.m_uRow});
    }

    // Scan each compressed row, then build a prefix sum of forbidden cells: those
    // that are neither red nor green. A rectangle is valid precisely when its
    // forbidden-cell count is zero.
    const auto uPrefixStride = vectorXBoundaries.size();
    std::vector<std::int64_t> vectorForbiddenPrefixSum(uPrefixStride * vectorYBoundaries.size(), 0);
    std::vector<int> vectorCoverageDeltas(uPrefixStride);
    std::vector<std::size_t> vectorCrossingColumns;
    for (std::size_t uRow = 0; uRow + 1 < vectorYBoundaries.size(); ++uRow) {
        std::ranges::fill(vectorCoverageDeltas, 0);
        vectorCrossingColumns.clear();
        const auto cover_columns_ = [&](std::size_t uFirstColumn, std::size_t uLastColumn) {
            ++vectorCoverageDeltas[uFirstColumn];
            --vectorCoverageDeltas[uLastColumn + 1];
        };
        for (const auto& boundarysegment : vectorBoundarySegments) {
            if (boundarysegment.m_bIsHorizontal) {
                if (uRow == boundarysegment.m_uFirstRow)
                    cover_columns_(boundarysegment.m_uFirstColumn, boundarysegment.m_uLastColumn);
            } else {
                if (uRow >= boundarysegment.m_uFirstRow && uRow <= boundarysegment.m_uLastRow)
                    cover_columns_(boundarysegment.m_uFirstColumn, boundarysegment.m_uFirstColumn);
                // Half-open vertical edges count each polygon vertex once.
                if (uRow >= boundarysegment.m_uFirstRow && uRow < boundarysegment.m_uLastRow)
                    vectorCrossingColumns.push_back(boundarysegment.m_uFirstColumn);
            }
        }
        std::ranges::sort(vectorCrossingColumns);
        if (vectorCrossingColumns.size() % 2 != 0)
            throw std::invalid_argument("Invalid polygon boundary");
        for (std::size_t uCrossingIndex = 0; uCrossingIndex < vectorCrossingColumns.size();
             uCrossingIndex += 2)
            cover_columns_(vectorCrossingColumns[uCrossingIndex],
                           vectorCrossingColumns[uCrossingIndex + 1]);
        int iRedGreenCoverage = 0;
        for (std::size_t uColumn = 0; uColumn + 1 < vectorXBoundaries.size(); ++uColumn) {
            iRedGreenCoverage += vectorCoverageDeltas[uColumn];
            vectorForbiddenPrefixSum[(uRow + 1) * uPrefixStride + uColumn + 1] =
                (iRedGreenCoverage == 0) +
                vectorForbiddenPrefixSum[uRow * uPrefixStride + uColumn + 1] +
                vectorForbiddenPrefixSum[(uRow + 1) * uPrefixStride + uColumn] -
                vectorForbiddenPrefixSum[uRow * uPrefixStride + uColumn];
        }
    }

    std::int64_t iLargestArea = 0;
    for (std::size_t uFirstTileIndex = 0; uFirstTileIndex < vectorCompressedRedTiles.size();
         ++uFirstTileIndex) {
        for (std::size_t uSecondTileIndex = uFirstTileIndex + 1;
             uSecondTileIndex < vectorCompressedRedTiles.size(); ++uSecondTileIndex) {
            const auto uFirstColumn =
                std::min(vectorCompressedRedTiles[uFirstTileIndex].m_uColumn,
                         vectorCompressedRedTiles[uSecondTileIndex].m_uColumn);
            const auto uColumnEnd = std::max(vectorCompressedRedTiles[uFirstTileIndex].m_uColumn,
                                             vectorCompressedRedTiles[uSecondTileIndex].m_uColumn) +
                                    1;
            const auto uFirstRow = std::min(vectorCompressedRedTiles[uFirstTileIndex].m_uRow,
                                            vectorCompressedRedTiles[uSecondTileIndex].m_uRow);
            const auto uRowEnd = std::max(vectorCompressedRedTiles[uFirstTileIndex].m_uRow,
                                          vectorCompressedRedTiles[uSecondTileIndex].m_uRow) +
                                 1;
            const auto iForbiddenCellCount =
                vectorForbiddenPrefixSum[uRowEnd * uPrefixStride + uColumnEnd] -
                vectorForbiddenPrefixSum[uFirstRow * uPrefixStride + uColumnEnd] -
                vectorForbiddenPrefixSum[uRowEnd * uPrefixStride + uFirstColumn] +
                vectorForbiddenPrefixSum[uFirstRow * uPrefixStride + uFirstColumn];
            if (iForbiddenCellCount == 0)
                iLargestArea = std::max(
                    iLargestArea,
                    RectangleArea(vectorXBoundaries[uColumnEnd] - vectorXBoundaries[uFirstColumn],
                                  vectorYBoundaries[uRowEnd] - vectorYBoundaries[uFirstRow]));
        }
    }
    return std::to_string(iLargestArea);
}
