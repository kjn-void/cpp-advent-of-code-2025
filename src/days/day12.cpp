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
            std::vector<int> mpishpcpreRequired;
            for (std::string usCount; inCounts >> usCount;) {
                const auto cpre = core::ValParseInteger<int>(usCount);
                if (cpre < 0)
                    throw std::invalid_argument("Shape counts must be nonnegative");
                mpishpcpreRequired.push_back(cpre);
            }
            if (mpishpcpreRequired.size() != rgshp_.size())
                throw std::invalid_argument("Expected one count per shape");
            rgreg_.push_back({ccol, crw, std::move(mpishpcpreRequired)});
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

    std::unordered_set<std::string> settxtOrientationKeys;
    std::vector<Ori> rgori;

    auto gridRotated = gridShape;
    for (int iterRotation = 0; iterRotation < 4; ++iterRotation) {
        if (iterRotation > 0)
            gridRotated = GridRotate(gridRotated);
        for (int iterReflection = 0; iterReflection < 2; ++iterReflection) {
            auto gridReflected = (iterReflection == 0) ? gridRotated : GridReflect(gridRotated);
            auto ori = OriFromGrid(gridReflected);
            if (!ori.rgdelta.empty()) {
                auto txtOrientationKey = TxtOrientationKey(ori);
                if (settxtOrientationKeys.insert(txtOrientationKey).second)
                    rgori.push_back(std::move(ori));
            }
        }
    }

    Shp shp;
    shp.rgori = std::move(rgori);
    if (!shp.rgori.empty())
        shp.areaOccupied = shp.rgori[0].rgdelta.size();
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

Day12::Ori Day12::OriFromGrid(const std::vector<std::vector<bool>>& gridSource) {
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

    Ori ori;
    ori.ccol = colLast - colFirst + 1;
    ori.crw = rwLast - rwFirst + 1;

    for (int rw = rwFirst; rw <= rwLast; ++rw)
        for (int col = colFirst; col <= colLast; ++col)
            if (gridSource[rw][col])
                ori.rgdelta.push_back({col - colFirst, rw - rwFirst});

    return ori;
}

std::string Day12::TxtOrientationKey(const Ori& ori) {
    std::ostringstream outKey;
    outKey << ori.ccol << "x" << ori.crw << ":";
    for (auto& delta : ori.rgdelta)
        outKey << delta.dxCell << "," << delta.dyCell << ";";
    return outKey.str();
}

// ------------------------------------------------------------
// Solver
// ------------------------------------------------------------

std::string Day12::TxtPart1() {
    int cregFitting = 0;
    for (auto& reg : rgreg_)
        if (FPresentsFit(reg))
            ++cregFitting;
    return std::to_string(cregFitting);
}

std::string Day12::TxtPart2() {
    return "0"; // Day 12 has no second computational puzzle.
}

bool Day12::FPresentsFit(const Reg& reg) const {
    const auto areaBoard = std::int64_t{reg.ccol} * reg.crw;
    std::int64_t areaRequired = 0;
    std::int64_t cpre = 0;
    int ccolSlot = 0, crwSlot = 0;
    for (std::size_t ishp = 0; ishp < rgshp_.size(); ++ishp) {
        if (reg.mpishpcpreRequired[ishp] == 0)
            continue;
        areaRequired += std::int64_t{reg.mpishpcpreRequired[ishp]} * rgshp_[ishp].areaOccupied;
        if (areaRequired > areaBoard)
            return false;
        if (!std::ranges::any_of(rgshp_[ishp].rgori, [&](const auto& ori) {
                return ori.ccol <= reg.ccol && ori.crw <= reg.crw;
            }))
            return false;
        cpre += reg.mpishpcpreRequired[ishp];
        const auto& ori = rgshp_[ishp].rgori.front();
        ccolSlot = std::max(ccolSlot, ori.ccol);
        crwSlot = std::max(crwSlot, ori.crw);
    }
    if (cpre == 0)
        return true;

    // A disjoint bounding box for every piece is a constructive proof of fit.
    const auto cpreCapacity = std::int64_t{reg.ccol / ccolSlot} * (reg.crw / crwSlot);
    if (cpre <= cpreCapacity)
        return true;
    return FPackRegion(reg);
}

// ------------------------------------------------------------
// Exact packing when area and bounding boxes do not decide the result
// ------------------------------------------------------------

bool Day12::FPackRegion(const Reg& reg) const {
    int ccol = reg.ccol, crw = reg.crw;
    std::vector<std::vector<std::vector<std::size_t>>> mpishprgplc(rgshp_.size());

    for (std::size_t ishp = 0; ishp < rgshp_.size(); ++ishp) {
        if (reg.mpishpcpreRequired[ishp] == 0)
            continue;
        for (const auto& ori : rgshp_[ishp].rgori) {
            for (int rwAnchor = 0; rwAnchor <= crw - ori.crw; ++rwAnchor)
                for (int colAnchor = 0; colAnchor <= ccol - ori.ccol; ++colAnchor) {
                    std::vector<std::size_t> plc;
                    for (auto& delta : ori.rgdelta)
                        plc.push_back(static_cast<std::size_t>(rwAnchor + delta.dyCell) * ccol +
                                      colAnchor + delta.dxCell);
                    mpishprgplc[ishp].push_back(std::move(plc));
                }
        }
    }

    std::vector<bool> mpicelfOccupied(static_cast<std::size_t>(ccol) * crw, false);
    auto mpishpcpreRemaining = reg.mpishpcpreRequired;
    std::vector<std::size_t> mpishpiplcFirst(rgshp_.size(), 0);
    return FPlaceRemaining(mpicelfOccupied, mpishpcpreRemaining, mpishprgplc, mpishpiplcFirst);
}

bool Day12::FPlaceRemaining(std::vector<bool>& mpicelfOccupied,
                            std::vector<int>& mpishpcpreRemaining,
                            const std::vector<std::vector<std::vector<std::size_t>>>& mpishprgplc,
                            std::vector<std::size_t>& mpishpiplcFirst) const {
    const auto ccelFree = std::ranges::count(mpicelfOccupied, false);

    std::int64_t areaRequired = 0;
    bool fComplete = true;
    for (std::size_t ishp = 0; ishp < mpishpcpreRemaining.size() && ishp < rgshp_.size(); ++ishp) {
        if (mpishpcpreRemaining[ishp] > 0) {
            fComplete = false;
            areaRequired += std::int64_t{mpishpcpreRemaining[ishp]} * rgshp_[ishp].areaOccupied;
        }
    }

    if (fComplete)
        return true;
    if (areaRequired > ccelFree)
        return false;

    std::size_t ishpBest = 0, cplcBest = std::numeric_limits<std::size_t>::max();

    for (std::size_t ishp = 0; ishp < mpishpcpreRemaining.size(); ++ishp) {
        if (mpishpcpreRemaining[ishp] <= 0)
            continue;
        std::size_t cplcFeasible = 0;
        for (std::size_t iplc = mpishpiplcFirst[ishp]; iplc < mpishprgplc[ishp].size(); ++iplc) {
            const auto& plc = mpishprgplc[ishp][iplc];
            if (std::all_of(plc.begin(), plc.end(),
                            [&](std::size_t icel) { return !mpicelfOccupied[icel]; })) {
                ++cplcFeasible;
                if (cplcFeasible >= cplcBest)
                    break;
            }
        }
        if (cplcFeasible == 0)
            return false;
        if (cplcFeasible < cplcBest) {
            cplcBest = cplcFeasible;
            ishpBest = ishp;
        }
    }

    mpishpcpreRemaining[ishpBest]--;
    const auto iplcFirst = mpishpiplcFirst[ishpBest];
    for (std::size_t iplc = iplcFirst; iplc < mpishprgplc[ishpBest].size(); ++iplc) {
        const auto& plc = mpishprgplc[ishpBest][iplc];
        if (std::all_of(plc.begin(), plc.end(),
                        [&](std::size_t icel) { return !mpicelfOccupied[icel]; })) {
            for (auto icel : plc)
                mpicelfOccupied[icel] = true;
            mpishpiplcFirst[ishpBest] = iplc + 1;
            if (FPlaceRemaining(mpicelfOccupied, mpishpcpreRemaining, mpishprgplc, mpishpiplcFirst))
                return true;
            for (auto icel : plc)
                mpicelfOccupied[icel] = false;
        }
    }
    mpishpiplcFirst[ishpBest] = iplcFirst;
    mpishpcpreRemaining[ishpBest]++;
    return false;
}
