#include "days/day12.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

// Registration
namespace {
const core::DayRegistration<Day12> dayregistration{12};
} // namespace

// ------------------------------------------------------------
// Parsing
// ------------------------------------------------------------

void Day12::SetInput(const std::vector<std::string>& vectorInputLines) {
    m_vectorPresentShapes.clear();
    m_vectorTreeRegions.clear();

    for (std::size_t uLineIndex = 0; uLineIndex < vectorInputLines.size();) {
        const auto stringLine = core::Trim(vectorInputLines[uLineIndex++]);
        if (stringLine.empty())
            continue;
        const auto uColonOffset = stringLine.find(':');
        if (uColonOffset == std::string_view::npos)
            throw std::invalid_argument("Expected shape or region header");
        const auto stringHeader = stringLine.substr(0, uColonOffset);
        const auto uDimensionSeparatorOffset = stringHeader.find('x');
        if (uDimensionSeparatorOffset == std::string_view::npos) {
            if (!m_vectorTreeRegions.empty() ||
                core::ParseInteger<std::size_t>(stringHeader) != m_vectorPresentShapes.size())
                throw std::invalid_argument("Shape IDs must be consecutive, starting at zero");
            std::vector<std::string> vectorShapeRows;
            while (uLineIndex < vectorInputLines.size()) {
                const auto stringInputRow = core::Trim(vectorInputLines[uLineIndex]);
                if (stringInputRow.empty() || stringInputRow.find(':') != std::string_view::npos)
                    break;
                if (stringInputRow.find_first_not_of(".#") != std::string_view::npos)
                    throw std::invalid_argument("Invalid shape cell");
                vectorShapeRows.emplace_back(stringInputRow);
                ++uLineIndex;
            }
            if (vectorShapeRows.empty())
                throw std::invalid_argument("Missing shape cells");
            auto presentshape = MakePresentShape(vectorShapeRows);
            if (presentshape.m_iOccupiedArea == 0)
                throw std::invalid_argument("Shape must occupy at least one cell");
            m_vectorPresentShapes.push_back(std::move(presentshape));
        } else {
            const auto iWidth =
                core::ParseInteger<int>(stringHeader.substr(0, uDimensionSeparatorOffset));
            const auto iHeight =
                core::ParseInteger<int>(stringHeader.substr(uDimensionSeparatorOffset + 1));
            if (iWidth <= 0 || iHeight <= 0)
                throw std::invalid_argument("Region dimensions must be positive");
            std::istringstream istringstreamCountsInput(
                std::string{stringLine.substr(uColonOffset + 1)});
            std::vector<int> vectorPresentCounts;
            for (std::string stringCount; istringstreamCountsInput >> stringCount;) {
                const auto iPresentCount = core::ParseInteger<int>(stringCount);
                if (iPresentCount < 0)
                    throw std::invalid_argument("Present counts must be nonnegative");
                vectorPresentCounts.push_back(iPresentCount);
            }
            if (vectorPresentCounts.size() != m_vectorPresentShapes.size())
                throw std::invalid_argument("Expected one present count per shape");
            m_vectorTreeRegions.push_back({iWidth, iHeight, std::move(vectorPresentCounts)});
        }
    }
}

// ------------------------------------------------------------
// Shape helpers
// ------------------------------------------------------------

Day12::PresentShape Day12::MakePresentShape(const std::vector<std::string>& vectorShapeRows) {
    int iHeight = vectorShapeRows.size();
    int iWidth = 0;
    for (auto& stringInputRow : vectorShapeRows)
        iWidth = std::max(iWidth, static_cast<int>(stringInputRow.size()));

    std::vector<std::vector<bool>> vectorShapeGrid(iHeight, std::vector<bool>(iWidth, false));
    for (int iRow = 0; iRow < iHeight; ++iRow)
        for (int iColumn = 0; iColumn < static_cast<int>(vectorShapeRows[iRow].size()); ++iColumn)
            if (vectorShapeRows[iRow][iColumn] == '#')
                vectorShapeGrid[iRow][iColumn] = true;

    std::unordered_set<std::string> setOrientationKeys;
    std::vector<PresentOrientation> vectorOrientations;

    auto vectorRotatedGrid = vectorShapeGrid;
    for (int iRotationIndex = 0; iRotationIndex < 4; ++iRotationIndex) {
        if (iRotationIndex > 0)
            vectorRotatedGrid = RotateClockwise(vectorRotatedGrid);
        for (int iReflectionIndex = 0; iReflectionIndex < 2; ++iReflectionIndex) {
            auto vectorReflectedGrid = (iReflectionIndex == 0)
                                           ? vectorRotatedGrid
                                           : ReflectHorizontally(vectorRotatedGrid);
            auto presentorientation = GridToOrientation(vectorReflectedGrid);
            if (!presentorientation.m_vectorCellOffsets.empty()) {
                auto stringOrientationKey = OrientationKey(presentorientation);
                if (setOrientationKeys.insert(stringOrientationKey).second)
                    vectorOrientations.push_back(std::move(presentorientation));
            }
        }
    }

    PresentShape presentshape;
    presentshape.m_vectorOrientations = std::move(vectorOrientations);
    if (!presentshape.m_vectorOrientations.empty())
        presentshape.m_iOccupiedArea =
            presentshape.m_vectorOrientations[0].m_vectorCellOffsets.size();
    return presentshape;
}

std::vector<std::vector<bool>>
Day12::RotateClockwise(const std::vector<std::vector<bool>>& vectorGrid) {
    int iHeight = vectorGrid.size();
    int iWidth = vectorGrid[0].size();
    std::vector<std::vector<bool>> vectorRotatedGrid(iWidth, std::vector<bool>(iHeight));
    for (int iRow = 0; iRow < iHeight; ++iRow)
        for (int iColumn = 0; iColumn < iWidth; ++iColumn)
            vectorRotatedGrid[iColumn][iHeight - 1 - iRow] = vectorGrid[iRow][iColumn];
    return vectorRotatedGrid;
}

std::vector<std::vector<bool>>
Day12::ReflectHorizontally(const std::vector<std::vector<bool>>& vectorGrid) {
    int iHeight = vectorGrid.size();
    int iWidth = vectorGrid[0].size();
    std::vector<std::vector<bool>> vectorReflectedGrid(iHeight, std::vector<bool>(iWidth));
    for (int iRow = 0; iRow < iHeight; ++iRow)
        for (int iColumn = 0; iColumn < iWidth; ++iColumn)
            vectorReflectedGrid[iRow][iWidth - 1 - iColumn] = vectorGrid[iRow][iColumn];
    return vectorReflectedGrid;
}

Day12::PresentOrientation
Day12::GridToOrientation(const std::vector<std::vector<bool>>& vectorGrid) {
    int iHeight = vectorGrid.size(), iWidth = vectorGrid[0].size();
    int iFirstColumn = iWidth, iFirstRow = iHeight, iLastColumn = -1, iLastRow = -1;

    for (int iRow = 0; iRow < iHeight; ++iRow)
        for (int iColumn = 0; iColumn < iWidth; ++iColumn)
            if (vectorGrid[iRow][iColumn]) {
                iFirstColumn = std::min(iFirstColumn, iColumn);
                iFirstRow = std::min(iFirstRow, iRow);
                iLastColumn = std::max(iLastColumn, iColumn);
                iLastRow = std::max(iLastRow, iRow);
            }

    if (iLastColumn < iFirstColumn)
        return {};

    PresentOrientation presentorientation;
    presentorientation.m_iWidth = iLastColumn - iFirstColumn + 1;
    presentorientation.m_iHeight = iLastRow - iFirstRow + 1;

    for (int iRow = iFirstRow; iRow <= iLastRow; ++iRow)
        for (int iColumn = iFirstColumn; iColumn <= iLastColumn; ++iColumn)
            if (vectorGrid[iRow][iColumn])
                presentorientation.m_vectorCellOffsets.push_back(
                    {iColumn - iFirstColumn, iRow - iFirstRow});

    return presentorientation;
}

std::string Day12::OrientationKey(const PresentOrientation& presentorientation) {
    std::ostringstream ostringstreamEncodedOrientation;
    ostringstreamEncodedOrientation << presentorientation.m_iWidth << "x"
                                    << presentorientation.m_iHeight << ":";
    for (auto& celloffset : presentorientation.m_vectorCellOffsets)
        ostringstreamEncodedOrientation << celloffset.m_iColumnOffset << ","
                                        << celloffset.m_iRowOffset << ";";
    return ostringstreamEncodedOrientation.str();
}

// ------------------------------------------------------------
// Solver
// ------------------------------------------------------------

std::string Day12::Part1() {
    int iFittingRegionCount = 0;
    for (auto& treeregion : m_vectorTreeRegions)
        if (PresentsFit(treeregion))
            ++iFittingRegionCount;
    return std::to_string(iFittingRegionCount);
}

std::string Day12::Part2() {
    // Day 12 has no second puzzle; its star is awarded once the other 23 are earned.
    return "0";
}

bool Day12::PresentsFit(const TreeRegion& treeregion) const {
    const auto iRegionArea = std::int64_t{treeregion.m_iWidth} * treeregion.m_iHeight;
    std::int64_t iRequiredArea = 0;
    std::int64_t iPresentCount = 0;
    int iSlotWidth = 0, iSlotHeight = 0;
    for (std::size_t uShapeIndex = 0; uShapeIndex < m_vectorPresentShapes.size(); ++uShapeIndex) {
        if (treeregion.m_vectorPresentCounts[uShapeIndex] == 0)
            continue;
        iRequiredArea += std::int64_t{treeregion.m_vectorPresentCounts[uShapeIndex]} *
                         m_vectorPresentShapes[uShapeIndex].m_iOccupiedArea;
        if (iRequiredArea > iRegionArea)
            return false;
        if (!std::ranges::any_of(m_vectorPresentShapes[uShapeIndex].m_vectorOrientations,
                                 [&](const auto& presentorientation) {
                                     return presentorientation.m_iWidth <= treeregion.m_iWidth &&
                                            presentorientation.m_iHeight <= treeregion.m_iHeight;
                                 }))
            return false;
        iPresentCount += treeregion.m_vectorPresentCounts[uShapeIndex];
        const auto& presentorientation =
            m_vectorPresentShapes[uShapeIndex].m_vectorOrientations.front();
        iSlotWidth = std::max(iSlotWidth, presentorientation.m_iWidth);
        iSlotHeight = std::max(iSlotHeight, presentorientation.m_iHeight);
    }
    if (iPresentCount == 0)
        return true;

    // A disjoint bounding box for every piece is a constructive proof of fit.
    const auto iSlotCount =
        std::int64_t{treeregion.m_iWidth / iSlotWidth} * (treeregion.m_iHeight / iSlotHeight);
    if (iPresentCount <= iSlotCount)
        return true;
    return CanPackRegion(treeregion);
}

// ------------------------------------------------------------
// Exact packing when area and bounding boxes do not decide the result
// ------------------------------------------------------------

bool Day12::CanPackRegion(const TreeRegion& treeregion) const {
    int iWidth = treeregion.m_iWidth, iHeight = treeregion.m_iHeight;
    std::vector<std::vector<std::vector<std::size_t>>> vectorPlacementsByShape(
        m_vectorPresentShapes.size());

    for (std::size_t uShapeIndex = 0; uShapeIndex < m_vectorPresentShapes.size(); ++uShapeIndex) {
        if (treeregion.m_vectorPresentCounts[uShapeIndex] == 0)
            continue;
        for (const auto& presentorientation :
             m_vectorPresentShapes[uShapeIndex].m_vectorOrientations) {
            for (int iAnchorRow = 0; iAnchorRow <= iHeight - presentorientation.m_iHeight;
                 ++iAnchorRow)
                for (int iAnchorColumn = 0; iAnchorColumn <= iWidth - presentorientation.m_iWidth;
                     ++iAnchorColumn) {
                    std::vector<std::size_t> vectorPlacement;
                    for (auto& celloffset : presentorientation.m_vectorCellOffsets)
                        vectorPlacement.push_back(
                            static_cast<std::size_t>(iAnchorRow + celloffset.m_iRowOffset) *
                                iWidth +
                            iAnchorColumn + celloffset.m_iColumnOffset);
                    vectorPlacementsByShape[uShapeIndex].push_back(std::move(vectorPlacement));
                }
        }
    }

    std::vector<bool> vectorOccupiedCells(static_cast<std::size_t>(iWidth) * iHeight, false);
    auto vectorRemainingCounts = treeregion.m_vectorPresentCounts;
    std::vector<std::size_t> vectorFirstPlacementByShape(m_vectorPresentShapes.size(), 0);
    return PlaceRemainingPresents(vectorOccupiedCells, vectorRemainingCounts,
                                  vectorPlacementsByShape, vectorFirstPlacementByShape);
}

bool Day12::PlaceRemainingPresents(
    std::vector<bool>& vectorOccupiedCells, std::vector<int>& vectorRemainingCounts,
    const std::vector<std::vector<std::vector<std::size_t>>>& vectorPlacementsByShape,
    std::vector<std::size_t>& vectorFirstPlacementByShape) const {
    const auto iFreeCellCount = std::ranges::count(vectorOccupiedCells, false);

    std::int64_t iRequiredArea = 0;
    bool bAllPresentsPlaced = true;
    for (std::size_t uShapeIndex = 0;
         uShapeIndex < vectorRemainingCounts.size() && uShapeIndex < m_vectorPresentShapes.size();
         ++uShapeIndex) {
        if (vectorRemainingCounts[uShapeIndex] > 0) {
            bAllPresentsPlaced = false;
            iRequiredArea += std::int64_t{vectorRemainingCounts[uShapeIndex]} *
                             m_vectorPresentShapes[uShapeIndex].m_iOccupiedArea;
        }
    }

    if (bAllPresentsPlaced)
        return true;
    if (iRequiredArea > iFreeCellCount)
        return false;

    std::size_t uChosenShapeIndex = 0,
                uFewestFeasiblePlacements = std::numeric_limits<std::size_t>::max();

    for (std::size_t uShapeIndex = 0; uShapeIndex < vectorRemainingCounts.size(); ++uShapeIndex) {
        if (vectorRemainingCounts[uShapeIndex] <= 0)
            continue;
        std::size_t uFeasiblePlacementCount = 0;
        for (std::size_t uPlacementIndex = vectorFirstPlacementByShape[uShapeIndex];
             uPlacementIndex < vectorPlacementsByShape[uShapeIndex].size(); ++uPlacementIndex) {
            const auto& vectorPlacement = vectorPlacementsByShape[uShapeIndex][uPlacementIndex];
            if (std::all_of(
                    vectorPlacement.begin(), vectorPlacement.end(),
                    [&](std::size_t uCellIndex) { return !vectorOccupiedCells[uCellIndex]; })) {
                ++uFeasiblePlacementCount;
                if (uFeasiblePlacementCount >= uFewestFeasiblePlacements)
                    break;
            }
        }
        if (uFeasiblePlacementCount == 0)
            return false;
        if (uFeasiblePlacementCount < uFewestFeasiblePlacements) {
            uFewestFeasiblePlacements = uFeasiblePlacementCount;
            uChosenShapeIndex = uShapeIndex;
        }
    }

    vectorRemainingCounts[uChosenShapeIndex]--;
    const auto uFirstPlacementIndex = vectorFirstPlacementByShape[uChosenShapeIndex];
    for (std::size_t uPlacementIndex = uFirstPlacementIndex;
         uPlacementIndex < vectorPlacementsByShape[uChosenShapeIndex].size(); ++uPlacementIndex) {
        const auto& vectorPlacement = vectorPlacementsByShape[uChosenShapeIndex][uPlacementIndex];
        if (std::all_of(vectorPlacement.begin(), vectorPlacement.end(),
                        [&](std::size_t uCellIndex) { return !vectorOccupiedCells[uCellIndex]; })) {
            for (auto uCellIndex : vectorPlacement)
                vectorOccupiedCells[uCellIndex] = true;
            vectorFirstPlacementByShape[uChosenShapeIndex] = uPlacementIndex + 1;
            if (PlaceRemainingPresents(vectorOccupiedCells, vectorRemainingCounts,
                                       vectorPlacementsByShape, vectorFirstPlacementByShape))
                return true;
            for (auto uCellIndex : vectorPlacement)
                vectorOccupiedCells[uCellIndex] = false;
        }
    }
    vectorFirstPlacementByShape[uChosenShapeIndex] = uFirstPlacementIndex;
    vectorRemainingCounts[uChosenShapeIndex]++;
    return false;
}
