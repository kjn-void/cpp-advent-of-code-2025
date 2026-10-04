#pragma once

#include "core/Solution.h"

#include <cstddef>
#include <string>
#include <vector>

class Day12 final : public Slv {
  public:
    void SetInput(const std::vector<std::string>& rgusLines) override;
    std::string TxtPart1() override;
    std::string TxtPart2() override;

  private:
    // ------------------------------------------------------------
    // Data types
    // ------------------------------------------------------------

    struct Delta {
        int dxCell, dyCell;
    };

    struct Ori {
        int ccol, crw;
        std::vector<Delta> rgdelta;
    };

    struct Shp {
        int areaOccupied = 0;
        std::vector<Ori> rgori;
    };

    struct Reg {
        int ccol, crw;
        std::vector<int> mpishpcpreRequired;
    };

    std::vector<Shp> rgshp_;
    std::vector<Reg> rgreg_;

    // ------------------------------------------------------------
    // Helpers
    // ------------------------------------------------------------

    static Shp ShpBuild(const std::vector<std::string>& rgusShapeRows);
    static std::vector<std::vector<bool>>
    GridRotate(const std::vector<std::vector<bool>>& gridSource);
    static std::vector<std::vector<bool>>
    GridReflect(const std::vector<std::vector<bool>>& gridSource);
    static Ori OriFromGrid(const std::vector<std::vector<bool>>& gridSource);
    static std::string TxtOrientationKey(const Ori& ori);

    bool FPresentsFit(const Reg& reg) const;

    bool FPackRegion(const Reg& reg) const;
    bool FPlaceRemaining(std::vector<bool>& mpicelfOccupied, std::vector<int>& mpishpcpreRemaining,
                         const std::vector<std::vector<std::vector<std::size_t>>>& mpishprgplc,
                         std::vector<std::size_t>& mpishpiplcFirst) const;
};
