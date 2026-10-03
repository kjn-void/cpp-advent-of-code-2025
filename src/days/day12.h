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

    struct Var {
        int ccol, crw;
        std::vector<Delta> rgdelta;
    };

    struct Shp {
        int areaOccupied = 0;
        std::vector<Var> rgvar;
    };

    struct Reg {
        int ccol, crw;
        std::vector<int> mpishpcnt;
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
    static Var VarFromGrid(const std::vector<std::vector<bool>>& gridSource);
    static std::string TxtVariantKey(const Var& var);

    bool FRegionFits(const Reg& reg) const;

    bool FCanPackRegion(const Reg& reg) const;
    bool FPack(std::vector<bool>& mpicelfOccupied, std::vector<int>& mpishpcnt,
               const std::vector<std::vector<std::vector<std::size_t>>>& mpishprgplc,
               std::vector<std::size_t>& mpishpiplcFirst) const;
};
