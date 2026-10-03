#include "days/day08.h"
#include "core/Register.h"

#include <algorithm>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <tuple>

// Registration
namespace {
const core::Drg<Day08> drgDay{8};
} // namespace

// -----------------------------------------------------------
// Parsing
// -----------------------------------------------------------

static Day08::Pt PtParse(const std::string& usLine) {
    std::stringstream inCoordinates(usLine);
    Day08::Pt ptParsed{};
    char chFirstComma{}, chSecondComma{};
    if (!(inCoordinates >> ptParsed.xJunction >> chFirstComma >> ptParsed.yJunction >>
          chSecondComma >> ptParsed.zJunction) ||
        chFirstComma != ',' || chSecondComma != ',' || !(inCoordinates >> std::ws).eof())
        throw std::invalid_argument("Expected three comma-separated coordinates");
    return ptParsed;
}

void Day08::SetInput(const std::vector<std::string>& rgusLines) {
    rgpt.clear();

    for (const auto& usLine : rgusLines) {
        if (!usLine.empty()) {
            rgpt.push_back(PtParse(usLine));
        }
    }

    rgedgConnections = RgedgBuildSorted(rgpt);
}

// -----------------------------------------------------------
// Distance & Edge Preparation
// -----------------------------------------------------------

std::int64_t Day08::DistSquared(const Pt& ptFirst, const Pt& ptSecond) {
    std::int64_t distSquared = 0;
    const auto fnAddSquaredDistance = [&](std::int64_t valFirstCoordinate,
                                          std::int64_t valSecondCoordinate) {
        // Unsigned subtraction also handles differences spanning the signed range.
        const auto distAxis =
            valFirstCoordinate >= valSecondCoordinate
                ? std::uint64_t(valFirstCoordinate) - std::uint64_t(valSecondCoordinate)
                : std::uint64_t(valSecondCoordinate) - std::uint64_t(valFirstCoordinate);
        if (distAxis > 3037000499ULL)
            throw std::overflow_error("Squared distance exceeds int64_t");
        const auto distAxisSquared = static_cast<std::int64_t>(distAxis * distAxis);
        if (distAxisSquared > std::numeric_limits<std::int64_t>::max() - distSquared)
            throw std::overflow_error("Squared distance exceeds int64_t");
        distSquared += distAxisSquared;
    };
    fnAddSquaredDistance(ptFirst.xJunction, ptSecond.xJunction);
    fnAddSquaredDistance(ptFirst.yJunction, ptSecond.yJunction);
    fnAddSquaredDistance(ptFirst.zJunction, ptSecond.zJunction);
    return distSquared;
}

std::vector<Day08::Edg> Day08::RgedgBuildSorted(std::span<const Pt> rgpt) {
    const int cpt = static_cast<int>(rgpt.size());
    std::vector<Edg> rgedgSorted;
    if (cpt > 1)
        rgedgSorted.reserve(rgpt.size() * (rgpt.size() - 1) / 2);

    for (int iptFirst = 0; iptFirst < cpt; ++iptFirst) {
        for (int iptSecond = iptFirst + 1; iptSecond < cpt; ++iptSecond) {
            rgedgSorted.push_back(
                {DistSquared(rgpt[iptFirst], rgpt[iptSecond]), iptFirst, iptSecond});
        }
    }

    std::ranges::sort(rgedgSorted, [](const Edg& edgFirst, const Edg& edgSecond) {
        return std::tie(edgFirst.distSquared, edgFirst.iptFirst, edgFirst.iptSecond) <
               std::tie(edgSecond.distSquared, edgSecond.iptFirst, edgSecond.iptSecond);
    });

    return rgedgSorted;
}

// -----------------------------------------------------------
// DSU
// -----------------------------------------------------------

Day08::Dsu::Dsu(int cpt) : mpiptiptParent(cpt), mpiptcntSize(cpt, 1) {
    for (int iptFirst = 0; iptFirst < cpt; ++iptFirst)
        mpiptiptParent[iptFirst] = iptFirst;
}

int Day08::Dsu::IptFind(int iptRoot) {
    while (mpiptiptParent[iptRoot] != iptRoot) {
        mpiptiptParent[iptRoot] = mpiptiptParent[mpiptiptParent[iptRoot]];
        iptRoot = mpiptiptParent[iptRoot];
    }
    return iptRoot;
}

bool Day08::Dsu::FUnite(int iptFirstRoot, int iptSecondRoot) {
    iptFirstRoot = IptFind(iptFirstRoot);
    iptSecondRoot = IptFind(iptSecondRoot);
    if (iptFirstRoot == iptSecondRoot)
        return false;

    if (mpiptcntSize[iptFirstRoot] < mpiptcntSize[iptSecondRoot])
        std::swap(iptFirstRoot, iptSecondRoot);
    mpiptiptParent[iptSecondRoot] = iptFirstRoot;
    mpiptcntSize[iptFirstRoot] += mpiptcntSize[iptSecondRoot];
    return true;
}

// -----------------------------------------------------------
// Core helpers
// -----------------------------------------------------------

std::vector<int> Day08::RgcntRunConnections(std::span<const Pt> rgpt,
                                            std::span<const Edg> rgedgConnections, int cedg) {
    if (rgpt.empty())
        return {};

    Dsu dsu(static_cast<int>(rgpt.size()));
    cedg = std::min(cedg, static_cast<int>(rgedgConnections.size()));

    for (int iedg = 0; iedg < cedg; ++iedg) {
        dsu.FUnite(rgedgConnections[iedg].iptFirst, rgedgConnections[iedg].iptSecond);
    }

    std::vector<int> rgcntCircuitSizes;
    for (int iptFirst = 0; iptFirst < static_cast<int>(rgpt.size()); ++iptFirst) {
        if (dsu.IptFind(iptFirst) == iptFirst)
            rgcntCircuitSizes.push_back(dsu.mpiptcntSize[iptFirst]);
    }

    std::ranges::sort(rgcntCircuitSizes, std::greater<>{});
    return rgcntCircuitSizes;
}

std::pair<int, int> Day08::LinkConnectAll(std::span<const Pt> rgpt,
                                          std::span<const Edg> rgedgConnections) {
    if (rgpt.size() < 2)
        return {0, 0};

    Dsu dsu(static_cast<int>(rgpt.size()));
    int cntCircuits = static_cast<int>(rgpt.size());
    int iptFirstLast = 0, iptSecondLast = 0;

    for (const auto& edg : rgedgConnections) {
        if (dsu.FUnite(edg.iptFirst, edg.iptSecond)) {
            --cntCircuits;
            iptFirstLast = edg.iptFirst;
            iptSecondLast = edg.iptSecond;
            if (cntCircuits == 1)
                break;
        }
    }

    return {iptFirstLast, iptSecondLast};
}

// -----------------------------------------------------------
// Parts
// -----------------------------------------------------------

std::string Day08::TxtPart1() {
    auto rgcntCircuitSizes = RgcntRunConnections(rgpt, rgedgConnections, 1000);
    if (rgcntCircuitSizes.size() < 3)
        return "0";

    std::int64_t valProductCircuitSizes = std::int64_t(rgcntCircuitSizes[0]) *
                                          std::int64_t(rgcntCircuitSizes[1]) *
                                          std::int64_t(rgcntCircuitSizes[2]);

    return std::to_string(valProductCircuitSizes);
}

std::string Day08::TxtPart2() {
    if (rgpt.size() < 2)
        return "0";

    auto [iptFirst, iptSecond] = LinkConnectAll(rgpt, rgedgConnections);
    const auto xFirst = rgpt[iptFirst].xJunction;
    const auto xSecond = rgpt[iptSecond].xJunction;
    const auto fnMagnitude = [](std::int64_t valSigned) {
        const auto maskSignedBits = static_cast<std::uint64_t>(valSigned);
        return valSigned < 0 ? std::uint64_t{0} - maskSignedBits : maskSignedBits;
    };
    const bool fNegative = (xFirst < 0) != (xSecond < 0);
    const auto valMagnitudeLast =
        static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + fNegative;
    const auto valFirstMagnitude = fnMagnitude(xFirst), valSecondMagnitude = fnMagnitude(xSecond);
    if (valSecondMagnitude != 0 && valFirstMagnitude > valMagnitudeLast / valSecondMagnitude)
        throw std::overflow_error("Junction coordinate product exceeds int64_t");
    const auto valProductMagnitude = valFirstMagnitude * valSecondMagnitude;
    if (fNegative && valProductMagnitude == valMagnitudeLast)
        return std::to_string(std::numeric_limits<std::int64_t>::min());
    const auto valProductSigned = static_cast<std::int64_t>(valProductMagnitude);
    return std::to_string(fNegative ? -valProductSigned : valProductSigned);
}
