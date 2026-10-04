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

static Day08::Jb JbParse(const std::string& usLine) {
    std::stringstream inCoordinates(usLine);
    Day08::Jb jb{};
    char chFirstComma{}, chSecondComma{};
    if (!(inCoordinates >> jb.x >> chFirstComma >> jb.y >> chSecondComma >> jb.z) ||
        chFirstComma != ',' || chSecondComma != ',' || !(inCoordinates >> std::ws).eof())
        throw std::invalid_argument("Expected three comma-separated coordinates");
    return jb;
}

void Day08::SetInput(const std::vector<std::string>& rgusLines) {
    rgjb.clear();

    for (const auto& usLine : rgusLines) {
        if (!usLine.empty()) {
            rgjb.push_back(JbParse(usLine));
        }
    }

    rgcn = RgcnBuildSorted(rgjb);
}

// -----------------------------------------------------------
// Distance & Edge Preparation
// -----------------------------------------------------------

std::int64_t Day08::DistSquared(const Jb& jbFirst, const Jb& jbSecond) {
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
    fnAddSquaredDistance(jbFirst.x, jbSecond.x);
    fnAddSquaredDistance(jbFirst.y, jbSecond.y);
    fnAddSquaredDistance(jbFirst.z, jbSecond.z);
    return distSquared;
}

std::vector<Day08::Cn> Day08::RgcnBuildSorted(std::span<const Jb> rgjb) {
    const int cjb = static_cast<int>(rgjb.size());
    std::vector<Cn> rgcnSorted;
    if (cjb > 1)
        rgcnSorted.reserve(rgjb.size() * (rgjb.size() - 1) / 2);

    for (int ijbFirst = 0; ijbFirst < cjb; ++ijbFirst) {
        for (int ijbSecond = ijbFirst + 1; ijbSecond < cjb; ++ijbSecond) {
            rgcnSorted.push_back(
                {DistSquared(rgjb[ijbFirst], rgjb[ijbSecond]), ijbFirst, ijbSecond});
        }
    }

    std::ranges::sort(rgcnSorted, [](const Cn& cnFirst, const Cn& cnSecond) {
        return std::tie(cnFirst.distSquared, cnFirst.ijbFirst, cnFirst.ijbSecond) <
               std::tie(cnSecond.distSquared, cnSecond.ijbFirst, cnSecond.ijbSecond);
    });

    return rgcnSorted;
}

// -----------------------------------------------------------
// DSU
// -----------------------------------------------------------

Day08::Dsu::Dsu(int cjb) : mpijbijbParent(cjb), mpijbcjbSize(cjb, 1) {
    for (int ijbFirst = 0; ijbFirst < cjb; ++ijbFirst)
        mpijbijbParent[ijbFirst] = ijbFirst;
}

int Day08::Dsu::IjbFindCircuit(int ijbRoot) {
    while (mpijbijbParent[ijbRoot] != ijbRoot) {
        mpijbijbParent[ijbRoot] = mpijbijbParent[mpijbijbParent[ijbRoot]];
        ijbRoot = mpijbijbParent[ijbRoot];
    }
    return ijbRoot;
}

bool Day08::Dsu::FUnite(int ijbFirstRoot, int ijbSecondRoot) {
    ijbFirstRoot = IjbFindCircuit(ijbFirstRoot);
    ijbSecondRoot = IjbFindCircuit(ijbSecondRoot);
    if (ijbFirstRoot == ijbSecondRoot)
        return false;

    if (mpijbcjbSize[ijbFirstRoot] < mpijbcjbSize[ijbSecondRoot])
        std::swap(ijbFirstRoot, ijbSecondRoot);
    mpijbijbParent[ijbSecondRoot] = ijbFirstRoot;
    mpijbcjbSize[ijbFirstRoot] += mpijbcjbSize[ijbSecondRoot];
    return true;
}

// -----------------------------------------------------------
// Core helpers
// -----------------------------------------------------------

std::vector<int> Day08::RgcjbConnectNearest(std::span<const Jb> rgjb, std::span<const Cn> rgcn,
                                            int ccn) {
    if (rgjb.empty())
        return {};

    Dsu dsu(static_cast<int>(rgjb.size()));
    ccn = std::min(ccn, static_cast<int>(rgcn.size()));

    for (int icn = 0; icn < ccn; ++icn) {
        dsu.FUnite(rgcn[icn].ijbFirst, rgcn[icn].ijbSecond);
    }

    std::vector<int> rgcjbCircuits;
    for (int ijbFirst = 0; ijbFirst < static_cast<int>(rgjb.size()); ++ijbFirst) {
        if (dsu.IjbFindCircuit(ijbFirst) == ijbFirst)
            rgcjbCircuits.push_back(dsu.mpijbcjbSize[ijbFirst]);
    }

    std::ranges::sort(rgcjbCircuits, std::greater<>{});
    return rgcjbCircuits;
}

std::pair<int, int> Day08::LinkConnectAll(std::span<const Jb> rgjb, std::span<const Cn> rgcn) {
    if (rgjb.size() < 2)
        return {0, 0};

    Dsu dsu(static_cast<int>(rgjb.size()));
    int ccir = static_cast<int>(rgjb.size());
    int ijbFirstLast = 0, ijbSecondLast = 0;

    for (const auto& cn : rgcn) {
        if (dsu.FUnite(cn.ijbFirst, cn.ijbSecond)) {
            --ccir;
            ijbFirstLast = cn.ijbFirst;
            ijbSecondLast = cn.ijbSecond;
            if (ccir == 1)
                break;
        }
    }

    return {ijbFirstLast, ijbSecondLast};
}

// -----------------------------------------------------------
// Parts
// -----------------------------------------------------------

std::string Day08::TxtPart1() {
    auto rgcjbCircuits = RgcjbConnectNearest(rgjb, rgcn, 1000);
    if (rgcjbCircuits.size() < 3)
        return "0";

    std::int64_t valProductCircuitSizes = std::int64_t(rgcjbCircuits[0]) *
                                          std::int64_t(rgcjbCircuits[1]) *
                                          std::int64_t(rgcjbCircuits[2]);

    return std::to_string(valProductCircuitSizes);
}

std::string Day08::TxtPart2() {
    if (rgjb.size() < 2)
        return "0";

    auto [ijbFirst, ijbSecond] = LinkConnectAll(rgjb, rgcn);
    const auto xFirst = rgjb[ijbFirst].x;
    const auto xSecond = rgjb[ijbSecond].x;
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
