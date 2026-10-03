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
const core::Drg<Day12> drgDay{12};
} // namespace

// ------------------------------------------------------------
// Parsing
// ------------------------------------------------------------

void Day12::SetInput(const std::vector<std::string>& rgusLines) {
    rgshp_.clear();
    rgreg_.clear();

    for (std::size_t iusLine = 0; iusLine < rgusLines.size();) {
        const auto usLine = core::UsTrim(rgusLines[iusLine++]);
        if (usLine.empty())
            continue;
        const auto offColon = usLine.find(':');
        if (offColon == std::string_view::npos)
            throw std::invalid_argument("Expected shape or region header");
        const auto usHeader = usLine.substr(0, offColon);
        const auto offDimensionSeparator = usHeader.find('x');
        if (offDimensionSeparator == std::string_view::npos) {
            if (!rgreg_.empty() || core::ValParseInteger<std::size_t>(usHeader) != rgshp_.size())
                throw std::invalid_argument("Shape IDs must be consecutive, starting at zero");
            std::vector<std::string> rgusShapeRows;
            while (iusLine < rgusLines.size()) {
                const auto usRow = core::UsTrim(rgusLines[iusLine]);
                if (usRow.empty() || usRow.find(':') != std::string_view::npos)
                    break;
                if (usRow.find_first_not_of(".#") != std::string_view::npos)
                    throw std::invalid_argument("Invalid shape cell");
                rgusShapeRows.emplace_back(usRow);
                ++iusLine;
            }
            if (rgusShapeRows.empty())
                throw std::invalid_argument("Missing shape cells");
            auto shpParsed = ShpBuild(rgusShapeRows);
            if (shpParsed.areaOccupied == 0)
                throw std::invalid_argument("Shape must occupy at least one cell");
            rgshp_.push_back(std::move(shpParsed));
        } else {
            const auto ccol = core::ValParseInteger<int>(usHeader.substr(0, offDimensionSeparator));
            const auto crw = core::ValParseInteger<int>(usHeader.substr(offDimensionSeparator + 1));
            if (ccol <= 0 || crw <= 0)
                throw std::invalid_argument("Region dimensions must be positive");
            std::istringstream inCounts(std::string{usLine.substr(offColon + 1)});
            std::vector<int> mpishpcnt;
            for (std::string usCount; inCounts >> usCount;) {
                const auto cntPieces = core::ValParseInteger<int>(usCount);
                if (cntPieces < 0)
                    throw std::invalid_argument("Shape counts must be nonnegative");
                mpishpcnt.push_back(cntPieces);
            }
            if (mpishpcnt.size() != rgshp_.size())
                throw std::invalid_argument("Expected one count per shape");
            rgreg_.push_back({ccol, crw, std::move(mpishpcnt)});
        }
    }
}

// ------------------------------------------------------------
// Shape helpers
// ------------------------------------------------------------

Day12::Shp Day12::ShpBuild(const std::vector<std::string>& rgusShapeRows) {
    int crw = rgusShapeRows.size();
    int ccol = 0;
    for (auto& usRow : rgusShapeRows)
        ccol = std::max(ccol, static_cast<int>(usRow.size()));

    std::vector<std::vector<bool>> gridShape(crw, std::vector<bool>(ccol, false));
    for (int rw = 0; rw < crw; ++rw)
        for (int col = 0; col < static_cast<int>(rgusShapeRows[rw].size()); ++col)
            if (rgusShapeRows[rw][col] == '#')
                gridShape[rw][col] = true;

    std::unordered_set<std::string> settxtVariantKeys;
    std::vector<Var> rgvar;

    auto gridRotated = gridShape;
    for (int iterRotation = 0; iterRotation < 4; ++iterRotation) {
        if (iterRotation > 0)
            gridRotated = GridRotate(gridRotated);
        for (int iterReflection = 0; iterReflection < 2; ++iterReflection) {
            auto gridReflected = (iterReflection == 0) ? gridRotated : GridReflect(gridRotated);
            auto var = VarFromGrid(gridReflected);
            if (!var.rgdelta.empty()) {
                auto txtVariantKey = TxtVariantKey(var);
                if (settxtVariantKeys.insert(txtVariantKey).second)
                    rgvar.push_back(std::move(var));
            }
        }
    }

    Shp shp;
    shp.rgvar = std::move(rgvar);
    if (!shp.rgvar.empty())
        shp.areaOccupied = shp.rgvar[0].rgdelta.size();
    return shp;
}

std::vector<std::vector<bool>> Day12::GridRotate(const std::vector<std::vector<bool>>& gridSource) {
    int crw = gridSource.size();
    int ccol = gridSource[0].size();
    std::vector<std::vector<bool>> gridResult(ccol, std::vector<bool>(crw));
    for (int rw = 0; rw < crw; ++rw)
        for (int col = 0; col < ccol; ++col)
            gridResult[col][crw - 1 - rw] = gridSource[rw][col];
    return gridResult;
}

std::vector<std::vector<bool>>
Day12::GridReflect(const std::vector<std::vector<bool>>& gridSource) {
    int crw = gridSource.size();
    int ccol = gridSource[0].size();
    std::vector<std::vector<bool>> gridResult(crw, std::vector<bool>(ccol));
    for (int rw = 0; rw < crw; ++rw)
        for (int col = 0; col < ccol; ++col)
            gridResult[rw][ccol - 1 - col] = gridSource[rw][col];
    return gridResult;
}

Day12::Var Day12::VarFromGrid(const std::vector<std::vector<bool>>& gridSource) {
    int crw = gridSource.size(), ccol = gridSource[0].size();
    int colFirst = ccol, rwFirst = crw, colLast = -1, rwLast = -1;

    for (int rw = 0; rw < crw; ++rw)
        for (int col = 0; col < ccol; ++col)
            if (gridSource[rw][col]) {
                colFirst = std::min(colFirst, col);
                rwFirst = std::min(rwFirst, rw);
                colLast = std::max(colLast, col);
                rwLast = std::max(rwLast, rw);
            }

    if (colLast < colFirst)
        return {};

    Var var;
    var.ccol = colLast - colFirst + 1;
    var.crw = rwLast - rwFirst + 1;

    for (int rw = rwFirst; rw <= rwLast; ++rw)
        for (int col = colFirst; col <= colLast; ++col)
            if (gridSource[rw][col])
                var.rgdelta.push_back({col - colFirst, rw - rwFirst});

    return var;
}

std::string Day12::TxtVariantKey(const Var& var) {
    std::ostringstream outKey;
    outKey << var.ccol << "x" << var.crw << ":";
    for (auto& delta : var.rgdelta)
        outKey << delta.dxCell << "," << delta.dyCell << ";";
    return outKey.str();
}

// ------------------------------------------------------------
// Solver
// ------------------------------------------------------------

std::string Day12::TxtPart1() {
    int cntFittingRegions = 0;
    for (auto& reg : rgreg_)
        if (FRegionFits(reg))
            ++cntFittingRegions;
    return std::to_string(cntFittingRegions);
}

std::string Day12::TxtPart2() {
    return "0"; // Day 12 has no second computational puzzle.
}

bool Day12::FRegionFits(const Reg& reg) const {
    const auto areaBoard = std::int64_t{reg.ccol} * reg.crw;
    std::int64_t areaRequired = 0;
    std::int64_t cntPieces = 0;
    int ccolSlots = 0, crwSlots = 0;
    for (std::size_t ishp = 0; ishp < rgshp_.size(); ++ishp) {
        if (reg.mpishpcnt[ishp] == 0)
            continue;
        areaRequired += std::int64_t{reg.mpishpcnt[ishp]} * rgshp_[ishp].areaOccupied;
        if (areaRequired > areaBoard)
            return false;
        if (!std::ranges::any_of(rgshp_[ishp].rgvar, [&](const auto& var) {
                return var.ccol <= reg.ccol && var.crw <= reg.crw;
            }))
            return false;
        cntPieces += reg.mpishpcnt[ishp];
        const auto& var = rgshp_[ishp].rgvar.front();
        ccolSlots = std::max(ccolSlots, var.ccol);
        crwSlots = std::max(crwSlots, var.crw);
    }
    if (cntPieces == 0)
        return true;

    // A disjoint bounding box for every piece is a constructive proof of fit.
    const auto cntSlots = std::int64_t{reg.ccol / ccolSlots} * (reg.crw / crwSlots);
    if (cntPieces <= cntSlots)
        return true;
    return FCanPackRegion(reg);
}

// ------------------------------------------------------------
// Exact packing when area and bounding boxes do not decide the result
// ------------------------------------------------------------

bool Day12::FCanPackRegion(const Reg& reg) const {
    int ccol = reg.ccol, crw = reg.crw;
    std::vector<std::vector<std::vector<std::size_t>>> mpishprgplc(rgshp_.size());

    for (std::size_t ishp = 0; ishp < rgshp_.size(); ++ishp) {
        if (reg.mpishpcnt[ishp] == 0)
            continue;
        for (const auto& var : rgshp_[ishp].rgvar) {
            for (int rwAnchor = 0; rwAnchor <= crw - var.crw; ++rwAnchor)
                for (int colAnchor = 0; colAnchor <= ccol - var.ccol; ++colAnchor) {
                    std::vector<std::size_t> plc;
                    for (auto& delta : var.rgdelta)
                        plc.push_back(static_cast<std::size_t>(rwAnchor + delta.dyCell) * ccol +
                                      colAnchor + delta.dxCell);
                    mpishprgplc[ishp].push_back(std::move(plc));
                }
        }
    }

    std::vector<bool> mpicelfOccupied(static_cast<std::size_t>(ccol) * crw, false);
    auto mpishpcnt = reg.mpishpcnt;
    std::vector<std::size_t> mpishpiplcFirst(rgshp_.size(), 0);
    return FPack(mpicelfOccupied, mpishpcnt, mpishprgplc, mpishpiplcFirst);
}

bool Day12::FPack(std::vector<bool>& mpicelfOccupied, std::vector<int>& mpishpcnt,
                  const std::vector<std::vector<std::vector<std::size_t>>>& mpishprgplc,
                  std::vector<std::size_t>& mpishpiplcFirst) const {
    const auto cntFreeCells = std::ranges::count(mpicelfOccupied, false);

    std::int64_t areaRequired = 0;
    bool fComplete = true;
    for (std::size_t ishp = 0; ishp < mpishpcnt.size() && ishp < rgshp_.size(); ++ishp) {
        if (mpishpcnt[ishp] > 0) {
            fComplete = false;
            areaRequired += std::int64_t{mpishpcnt[ishp]} * rgshp_[ishp].areaOccupied;
        }
    }

    if (fComplete)
        return true;
    if (areaRequired > cntFreeCells)
        return false;

    std::size_t ishpBest = 0, cntBestPlacements = std::numeric_limits<std::size_t>::max();

    for (std::size_t ishp = 0; ishp < mpishpcnt.size(); ++ishp) {
        if (mpishpcnt[ishp] <= 0)
            continue;
        std::size_t cntFeasiblePlacements = 0;
        for (std::size_t iplc = mpishpiplcFirst[ishp]; iplc < mpishprgplc[ishp].size(); ++iplc) {
            const auto& plc = mpishprgplc[ishp][iplc];
            if (std::all_of(plc.begin(), plc.end(),
                            [&](std::size_t icel) { return !mpicelfOccupied[icel]; })) {
                ++cntFeasiblePlacements;
                if (cntFeasiblePlacements >= cntBestPlacements)
                    break;
            }
        }
        if (cntFeasiblePlacements == 0)
            return false;
        if (cntFeasiblePlacements < cntBestPlacements) {
            cntBestPlacements = cntFeasiblePlacements;
            ishpBest = ishp;
        }
    }

    mpishpcnt[ishpBest]--;
    const auto iplcFirst = mpishpiplcFirst[ishpBest];
    for (std::size_t iplc = iplcFirst; iplc < mpishprgplc[ishpBest].size(); ++iplc) {
        const auto& plc = mpishprgplc[ishpBest][iplc];
        if (std::all_of(plc.begin(), plc.end(),
                        [&](std::size_t icel) { return !mpicelfOccupied[icel]; })) {
            for (auto icel : plc)
                mpicelfOccupied[icel] = true;
            mpishpiplcFirst[ishpBest] = iplc + 1;
            if (FPack(mpicelfOccupied, mpishpcnt, mpishprgplc, mpishpiplcFirst))
                return true;
            for (auto icel : plc)
                mpicelfOccupied[icel] = false;
        }
    }
    mpishpiplcFirst[ishpBest] = iplcFirst;
    mpishpcnt[ishpBest]++;
    return false;
}
