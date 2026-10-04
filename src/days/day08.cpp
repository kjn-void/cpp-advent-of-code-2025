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

    rgcnByDistance = RgcnSortByDistance(rgjb);
}

// -----------------------------------------------------------
// Distances and candidate connections
// -----------------------------------------------------------

std::int64_t Day08::DistSquared(const Jb& jbFirst, const Jb& jbSecond) {
    std::int64_t distSquared = 0;
    const auto fnAddSquaredDistance = [&](std::int64_t xyFirst, std::int64_t xySecond) {
        // Unsigned subtraction also handles differences spanning the signed range.
        const auto distAxis = xyFirst >= xySecond
                                  ? std::uint64_t(xyFirst) - std::uint64_t(xySecond)
                                  : std::uint64_t(xySecond) - std::uint64_t(xyFirst);
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

std::vector<Day08::Cn> Day08::RgcnSortByDistance(std::span<const Jb> rgjb) {
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
// Union-find
// -----------------------------------------------------------

Day08::Dsu::Dsu(int cjb) : mpijbijbParent(cjb), mpijbcjbSize(cjb, 1) {
    for (int ijb = 0; ijb < cjb; ++ijb)
        mpijbijbParent[ijb] = ijb;
}

int Day08::Dsu::IjbFindRoot(int ijb) {
    while (mpijbijbParent[ijb] != ijb) {
        mpijbijbParent[ijb] = mpijbijbParent[mpijbijbParent[ijb]];
        ijb = mpijbijbParent[ijb];
    }
    return ijb;
}

bool Day08::Dsu::FUnite(int ijbFirst, int ijbSecond) {
    int ijbFirstRoot = IjbFindRoot(ijbFirst);
    int ijbSecondRoot = IjbFindRoot(ijbSecond);
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

std::vector<int> Day08::RgcjbConnectNearest(std::span<const Jb> rgjb,
                                            std::span<const Cn> rgcnByDistance, int ccn) {
    if (rgjb.empty())
        return {};

    Dsu dsu(static_cast<int>(rgjb.size()));
    ccn = std::min(ccn, static_cast<int>(rgcnByDistance.size()));

    for (int icn = 0; icn < ccn; ++icn) {
        dsu.FUnite(rgcnByDistance[icn].ijbFirst, rgcnByDistance[icn].ijbSecond);
    }

    std::vector<int> rgcjbCircuits;
    for (int ijb = 0; ijb < static_cast<int>(rgjb.size()); ++ijb) {
        if (dsu.IjbFindRoot(ijb) == ijb)
            rgcjbCircuits.push_back(dsu.mpijbcjbSize[ijb]);
    }

    std::ranges::sort(rgcjbCircuits, std::greater<>{});
    return rgcjbCircuits;
}

Day08::Cn Day08::CnConnectAll(std::span<const Jb> rgjb, std::span<const Cn> rgcnByDistance) {
    if (rgjb.size() < 2)
        return {0, 0, 0};

    Dsu dsu(static_cast<int>(rgjb.size()));
    int ccir = static_cast<int>(rgjb.size());
    Cn cnFinal{0, 0, 0};

    for (const auto& cn : rgcnByDistance) {
        if (dsu.FUnite(cn.ijbFirst, cn.ijbSecond)) {
            --ccir;
            cnFinal = cn;
            if (ccir == 1)
                break;
        }
    }

    return cnFinal;
}

// -----------------------------------------------------------
// Parts
// -----------------------------------------------------------

std::string Day08::TxtPart1() {
    auto rgcjbCircuits = RgcjbConnectNearest(rgjb, rgcnByDistance, 1000);
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

    const auto cnFinal = CnConnectAll(rgjb, rgcnByDistance);
    const auto xFirst = rgjb[cnFinal.ijbFirst].x;
    const auto xSecond = rgjb[cnFinal.ijbSecond].x;
    const auto fnMagnitude = [](std::int64_t valSigned) {
        const auto maskSignedBits = static_cast<std::uint64_t>(valSigned);
        return valSigned < 0 ? std::uint64_t{0} - maskSignedBits : maskSignedBits;
    };
    const bool fProductNegative = (xFirst < 0) != (xSecond < 0);
    const auto valMagnitudeLast =
        static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + fProductNegative;
    const auto valFirstMagnitude = fnMagnitude(xFirst), valSecondMagnitude = fnMagnitude(xSecond);
    if (valSecondMagnitude != 0 && valFirstMagnitude > valMagnitudeLast / valSecondMagnitude)
        throw std::overflow_error("X coordinate product exceeds int64_t");
    const auto valProductMagnitude = valFirstMagnitude * valSecondMagnitude;
    if (fProductNegative && valProductMagnitude == valMagnitudeLast)
        return std::to_string(std::numeric_limits<std::int64_t>::min());
    const auto valProductSigned = static_cast<std::int64_t>(valProductMagnitude);
    return std::to_string(fProductNegative ? -valProductSigned : valProductSigned);
}
