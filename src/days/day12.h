#pragma once

#include "core/Solution.h"

#include <cstddef>
#include <string>
#include <vector>

class Day12 final : public Solution {
  public:
    void SetInput(const std::vector<std::string>& vectorInputLines) override;
    std::string Part1() override;
    std::string Part2() override;

  private:
    // ------------------------------------------------------------
    // Data types
    // ------------------------------------------------------------

    struct CellOffset {
        int m_iColumnOffset, m_iRowOffset;
    };

    struct PresentOrientation {
        int m_iWidth, m_iHeight;
        std::vector<CellOffset> m_vectorCellOffsets;
    };

    struct PresentShape {
        int m_iOccupiedArea = 0;
        std::vector<PresentOrientation> m_vectorOrientations;
    };

    struct TreeRegion {
        int m_iWidth, m_iHeight;
        std::vector<int> m_vectorPresentCounts;
    };

    std::vector<PresentShape> m_vectorPresentShapes;
    std::vector<TreeRegion> m_vectorTreeRegions;

    // ------------------------------------------------------------
    // Helpers
    // ------------------------------------------------------------

    static PresentShape MakePresentShape(const std::vector<std::string>& vectorShapeRows);
    static std::vector<std::vector<bool>>
    RotateClockwise(const std::vector<std::vector<bool>>& vectorGrid);
    static std::vector<std::vector<bool>>
    ReflectHorizontally(const std::vector<std::vector<bool>>& vectorGrid);
    static PresentOrientation GridToOrientation(const std::vector<std::vector<bool>>& vectorGrid);
    static std::string OrientationKey(const PresentOrientation& presentorientation);

    bool PresentsFit(const TreeRegion& treeregion) const;

    bool CanPackRegion(const TreeRegion& treeregion) const;
    bool PlaceRemainingPresents(
        std::vector<bool>& vectorOccupiedCells, std::vector<int>& vectorRemainingCounts,
        const std::vector<std::vector<std::vector<std::size_t>>>& vectorPlacementsByShape,
        std::vector<std::size_t>& vectorFirstPlacementByShape) const;
};
