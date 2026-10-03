#include "days/day09.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <stdexcept>

// Registration
namespace {
const core::Drg<Day09> drgDay{9};

std::int64_t AreaRectangle(std::int64_t lenWidth, std::int64_t lenHeight) {
    if (lenWidth > std::numeric_limits<std::int64_t>::max() / lenHeight)
        throw std::overflow_error("Rectangle area exceeds int64_t");
    return lenWidth * lenHeight;
}
} // namespace

// ----------------------------------------------------------
// Input
// ----------------------------------------------------------

void Day09::SetInput(const std::vector<std::string>& rgusLines) {
    rgptRed_.clear();

    for (const auto& usLine : rgusLines) {
        if (usLine.empty())
            continue;
        auto offComma = usLine.find(',');
        if (offComma == std::string::npos)
            throw std::invalid_argument("Expected a coordinate pair");
        const auto xTile = core::ValParseInteger<int>(std::string_view(usLine).substr(0, offComma));
        const auto yTile =
            core::ValParseInteger<int>(std::string_view(usLine).substr(offComma + 1));
        rgptRed_.push_back({xTile, yTile});
    }
}

// ----------------------------------------------------------
// Part 1
// ----------------------------------------------------------

std::string Day09::TxtPart1() {
    return std::to_string(AreaMaxInclusive(rgptRed_));
}

std::int64_t Day09::AreaMaxInclusive(const std::vector<Pt>& rgpt) {
    int cpt = static_cast<int>(rgpt.size());
    std::int64_t areaLargest = 0;

    for (int iptFirst = 0; iptFirst < cpt; ++iptFirst) {
        for (int iptSecond = iptFirst + 1; iptSecond < cpt; ++iptSecond) {
            std::int64_t lenWidth =
                std::abs(std::int64_t{rgpt[iptFirst].xTile} - rgpt[iptSecond].xTile) + 1;
            std::int64_t lenHeight =
                std::abs(std::int64_t{rgpt[iptFirst].yTile} - rgpt[iptSecond].yTile) + 1;
            areaLargest = std::max(areaLargest, AreaRectangle(lenWidth, lenHeight));
        }
    }
    return areaLargest;
}

// ----------------------------------------------------------
// Part 2
// ----------------------------------------------------------

std::string Day09::TxtPart2() {
    if (rgptRed_.size() < 2)
        return "0";

    // Each boundary coordinate and its successor start a distinct interval of
    // integer tiles. Interior gaps can be represented by a single compressed cell.
    std::vector<std::int64_t> rgx, rgy;
    for (const auto& ptRed : rgptRed_) {
        rgx.push_back(ptRed.xTile);
        rgx.push_back(std::int64_t{ptRed.xTile} + 1);
        rgy.push_back(ptRed.yTile);
        rgy.push_back(std::int64_t{ptRed.yTile} + 1);
    }
    const auto fnCompress = [](auto& rgxy) {
        std::ranges::sort(rgxy);
        const auto rngDuplicates = std::ranges::unique(rgxy);
        rgxy.erase(rngDuplicates.begin(), rngDuplicates.end());
    };
    fnCompress(rgx);
    fnCompress(rgy);

    struct Cel {
        std::size_t colCompressed, rwCompressed;
    };
    std::vector<Cel> rgcelVertices;
    for (const auto& ptRed : rgptRed_) {
        rgcelVertices.push_back(
            {static_cast<std::size_t>(std::ranges::lower_bound(rgx, ptRed.xTile) - rgx.begin()),
             static_cast<std::size_t>(std::ranges::lower_bound(rgy, ptRed.yTile) - rgy.begin())});
    }
    struct Seg {
        std::size_t colFirst, colLast, rwFirst, rwLast;
        bool fHorizontal;
    };
    std::vector<Seg> rgseg;
    for (std::size_t icelFirst = 0; icelFirst < rgcelVertices.size(); ++icelFirst) {
        const auto celFirst = rgcelVertices[icelFirst];
        const auto celSecond = rgcelVertices[(icelFirst + 1) % rgcelVertices.size()];
        if (celFirst.colCompressed != celSecond.colCompressed &&
            celFirst.rwCompressed != celSecond.rwCompressed)
            throw std::invalid_argument("Polygon edges must be axis-aligned");
        rgseg.push_back({std::min(celFirst.colCompressed, celSecond.colCompressed),
                         std::max(celFirst.colCompressed, celSecond.colCompressed),
                         std::min(celFirst.rwCompressed, celSecond.rwCompressed),
                         std::max(celFirst.rwCompressed, celSecond.rwCompressed),
                         celFirst.rwCompressed == celSecond.rwCompressed});
    }

    // Scan each compressed row, then build a prefix sum of forbidden cells.
    // A rectangle is valid precisely when its forbidden-cell count is zero.
    const auto ccolPrefix = rgx.size();
    std::vector<std::int64_t> gridForbiddenPrefix(ccolPrefix * rgy.size(), 0);
    std::vector<int> mpcolcntDelta(ccolPrefix);
    std::vector<std::size_t> rgcolCrossings;
    for (std::size_t rwCompressed = 0; rwCompressed + 1 < rgy.size(); ++rwCompressed) {
        std::ranges::fill(mpcolcntDelta, 0);
        rgcolCrossings.clear();
        const auto fnCoverInterval = [&](std::size_t colFirst, std::size_t colLast) {
            ++mpcolcntDelta[colFirst];
            --mpcolcntDelta[colLast + 1];
        };
        for (const auto& seg : rgseg) {
            if (seg.fHorizontal) {
                if (rwCompressed == seg.rwFirst)
                    fnCoverInterval(seg.colFirst, seg.colLast);
            } else {
                if (rwCompressed >= seg.rwFirst && rwCompressed <= seg.rwLast)
                    fnCoverInterval(seg.colFirst, seg.colFirst);
                // Half-open vertical edges count each polygon vertex once.
                if (rwCompressed >= seg.rwFirst && rwCompressed < seg.rwLast)
                    rgcolCrossings.push_back(seg.colFirst);
            }
        }
        std::ranges::sort(rgcolCrossings);
        if (rgcolCrossings.size() % 2 != 0)
            throw std::invalid_argument("Invalid polygon boundary");
        for (std::size_t icolCrossing = 0; icolCrossing < rgcolCrossings.size(); icolCrossing += 2)
            fnCoverInterval(rgcolCrossings[icolCrossing], rgcolCrossings[icolCrossing + 1]);
        int cntCoveringIntervals = 0;
        for (std::size_t colCompressed = 0; colCompressed + 1 < rgx.size(); ++colCompressed) {
            cntCoveringIntervals += mpcolcntDelta[colCompressed];
            gridForbiddenPrefix[(rwCompressed + 1) * ccolPrefix + colCompressed + 1] =
                (cntCoveringIntervals == 0) +
                gridForbiddenPrefix[rwCompressed * ccolPrefix + colCompressed + 1] +
                gridForbiddenPrefix[(rwCompressed + 1) * ccolPrefix + colCompressed] -
                gridForbiddenPrefix[rwCompressed * ccolPrefix + colCompressed];
        }
    }

    std::int64_t areaLargest = 0;
    for (std::size_t icelFirst = 0; icelFirst < rgcelVertices.size(); ++icelFirst) {
        for (std::size_t icelSecond = icelFirst + 1; icelSecond < rgcelVertices.size();
             ++icelSecond) {
            const auto colFirst = std::min(rgcelVertices[icelFirst].colCompressed,
                                           rgcelVertices[icelSecond].colCompressed);
            const auto colLim = std::max(rgcelVertices[icelFirst].colCompressed,
                                         rgcelVertices[icelSecond].colCompressed) +
                                1;
            const auto rwFirst = std::min(rgcelVertices[icelFirst].rwCompressed,
                                          rgcelVertices[icelSecond].rwCompressed);
            const auto rwLim = std::max(rgcelVertices[icelFirst].rwCompressed,
                                        rgcelVertices[icelSecond].rwCompressed) +
                               1;
            const auto cntForbiddenCells = gridForbiddenPrefix[rwLim * ccolPrefix + colLim] -
                                           gridForbiddenPrefix[rwFirst * ccolPrefix + colLim] -
                                           gridForbiddenPrefix[rwLim * ccolPrefix + colFirst] +
                                           gridForbiddenPrefix[rwFirst * ccolPrefix + colFirst];
            if (cntForbiddenCells == 0)
                areaLargest = std::max(areaLargest, AreaRectangle(rgx[colLim] - rgx[colFirst],
                                                                  rgy[rwLim] - rgy[rwFirst]));
        }
    }
    return std::to_string(areaLargest);
}
