#include "days/day08.h"
#include "core/Register.h"

#include <algorithm>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <tuple>

// Registration
namespace {
const core::DayRegistration<Day08> dayregistration{8};
} // namespace

// -----------------------------------------------------------
// Parsing
// -----------------------------------------------------------

static Day08::JunctionBox ParseJunctionBox(const std::string& stringLine) {
    std::stringstream stringstreamCoordinates(stringLine);
    Day08::JunctionBox junctionbox{};
    char iFirstComma{}, iSecondComma{};
    if (!(stringstreamCoordinates >> junctionbox.m_iX >> iFirstComma >> junctionbox.m_iY >>
          iSecondComma >> junctionbox.m_iZ) ||
        iFirstComma != ',' || iSecondComma != ',' || !(stringstreamCoordinates >> std::ws).eof())
        throw std::invalid_argument("Expected three comma-separated coordinates");
    return junctionbox;
}

void Day08::SetInput(const std::vector<std::string>& vectorInputLines) {
    m_vectorJunctionBoxes.clear();

    for (const auto& stringLine : vectorInputLines) {
        if (!stringLine.empty()) {
            m_vectorJunctionBoxes.push_back(ParseJunctionBox(stringLine));
        }
    }

    m_vectorPairsByDistance = SortPairsByDistance(m_vectorJunctionBoxes);
}

// -----------------------------------------------------------
// Distances and candidate pairs
// -----------------------------------------------------------

std::int64_t Day08::SquaredDistance(const JunctionBox& junctionboxFirst,
                                    const JunctionBox& junctionboxSecond) {
    std::int64_t iSquaredDistance = 0;
    const auto add_squared_axis_distance_ = [&](std::int64_t iFirstCoordinate,
                                                std::int64_t iSecondCoordinate) {
        // Unsigned subtraction also handles differences spanning the signed range.
        const auto uAxisDistance =
            iFirstCoordinate >= iSecondCoordinate
                ? std::uint64_t(iFirstCoordinate) - std::uint64_t(iSecondCoordinate)
                : std::uint64_t(iSecondCoordinate) - std::uint64_t(iFirstCoordinate);
        if (uAxisDistance > 3037000499ULL)
            throw std::overflow_error("Squared distance exceeds int64_t");
        const auto iSquaredAxisDistance = static_cast<std::int64_t>(uAxisDistance * uAxisDistance);
        if (iSquaredAxisDistance > std::numeric_limits<std::int64_t>::max() - iSquaredDistance)
            throw std::overflow_error("Squared distance exceeds int64_t");
        iSquaredDistance += iSquaredAxisDistance;
    };
    add_squared_axis_distance_(junctionboxFirst.m_iX, junctionboxSecond.m_iX);
    add_squared_axis_distance_(junctionboxFirst.m_iY, junctionboxSecond.m_iY);
    add_squared_axis_distance_(junctionboxFirst.m_iZ, junctionboxSecond.m_iZ);
    return iSquaredDistance;
}

std::vector<Day08::BoxPair>
Day08::SortPairsByDistance(std::span<const JunctionBox> spanJunctionBoxes) {
    const int iBoxCount = static_cast<int>(spanJunctionBoxes.size());
    std::vector<BoxPair> vectorPairsByDistance;
    if (iBoxCount > 1)
        vectorPairsByDistance.reserve(spanJunctionBoxes.size() * (spanJunctionBoxes.size() - 1) /
                                      2);

    for (int iFirstBoxIndex = 0; iFirstBoxIndex < iBoxCount; ++iFirstBoxIndex) {
        for (int iSecondBoxIndex = iFirstBoxIndex + 1; iSecondBoxIndex < iBoxCount;
             ++iSecondBoxIndex) {
            vectorPairsByDistance.push_back({SquaredDistance(spanJunctionBoxes[iFirstBoxIndex],
                                                             spanJunctionBoxes[iSecondBoxIndex]),
                                             iFirstBoxIndex, iSecondBoxIndex});
        }
    }

    std::ranges::sort(vectorPairsByDistance, [](const BoxPair& boxpairFirst,
                                                const BoxPair& boxpairSecond) {
        return std::tie(boxpairFirst.m_iSquaredDistance, boxpairFirst.m_iFirstBoxIndex,
                        boxpairFirst.m_iSecondBoxIndex) < std::tie(boxpairSecond.m_iSquaredDistance,
                                                                   boxpairSecond.m_iFirstBoxIndex,
                                                                   boxpairSecond.m_iSecondBoxIndex);
    });

    return vectorPairsByDistance;
}

// -----------------------------------------------------------
// Union-find
// -----------------------------------------------------------

Day08::CircuitSet::CircuitSet(int iBoxCount)
    : m_vectorParentByBox(iBoxCount), m_vectorBoxCountByRoot(iBoxCount, 1) {
    for (int iBoxIndex = 0; iBoxIndex < iBoxCount; ++iBoxIndex)
        m_vectorParentByBox[iBoxIndex] = iBoxIndex;
}

int Day08::CircuitSet::FindCircuitRoot(int iBoxIndex) {
    while (m_vectorParentByBox[iBoxIndex] != iBoxIndex) {
        m_vectorParentByBox[iBoxIndex] = m_vectorParentByBox[m_vectorParentByBox[iBoxIndex]];
        iBoxIndex = m_vectorParentByBox[iBoxIndex];
    }
    return iBoxIndex;
}

bool Day08::CircuitSet::JoinCircuits(int iFirstBoxIndex, int iSecondBoxIndex) {
    int iFirstRoot = FindCircuitRoot(iFirstBoxIndex);
    int iSecondRoot = FindCircuitRoot(iSecondBoxIndex);
    if (iFirstRoot == iSecondRoot)
        return false;

    if (m_vectorBoxCountByRoot[iFirstRoot] < m_vectorBoxCountByRoot[iSecondRoot])
        std::swap(iFirstRoot, iSecondRoot);
    m_vectorParentByBox[iSecondRoot] = iFirstRoot;
    m_vectorBoxCountByRoot[iFirstRoot] += m_vectorBoxCountByRoot[iSecondRoot];
    return true;
}

// -----------------------------------------------------------
// Core helpers
// -----------------------------------------------------------

std::vector<int> Day08::CircuitSizesAfterConnections(std::span<const JunctionBox> spanJunctionBoxes,
                                                     std::span<const BoxPair> spanPairsByDistance,
                                                     int iConnectionCount) {
    if (spanJunctionBoxes.empty())
        return {};

    CircuitSet circuitset(static_cast<int>(spanJunctionBoxes.size()));
    iConnectionCount = std::min(iConnectionCount, static_cast<int>(spanPairsByDistance.size()));

    for (int iPairIndex = 0; iPairIndex < iConnectionCount; ++iPairIndex) {
        circuitset.JoinCircuits(spanPairsByDistance[iPairIndex].m_iFirstBoxIndex,
                                spanPairsByDistance[iPairIndex].m_iSecondBoxIndex);
    }

    std::vector<int> vectorCircuitSizes;
    for (int iBoxIndex = 0; iBoxIndex < static_cast<int>(spanJunctionBoxes.size()); ++iBoxIndex) {
        if (circuitset.FindCircuitRoot(iBoxIndex) == iBoxIndex)
            vectorCircuitSizes.push_back(circuitset.m_vectorBoxCountByRoot[iBoxIndex]);
    }

    std::ranges::sort(vectorCircuitSizes, std::greater<>{});
    return vectorCircuitSizes;
}

Day08::BoxPair Day08::ConnectAllJunctionBoxes(std::span<const JunctionBox> spanJunctionBoxes,
                                              std::span<const BoxPair> spanPairsByDistance) {
    if (spanJunctionBoxes.size() < 2)
        return {0, 0, 0};

    CircuitSet circuitset(static_cast<int>(spanJunctionBoxes.size()));
    int iCircuitCount = static_cast<int>(spanJunctionBoxes.size());
    BoxPair boxpairFinalConnection{0, 0, 0};

    for (const auto& boxpair : spanPairsByDistance) {
        if (circuitset.JoinCircuits(boxpair.m_iFirstBoxIndex, boxpair.m_iSecondBoxIndex)) {
            --iCircuitCount;
            boxpairFinalConnection = boxpair;
            if (iCircuitCount == 1)
                break;
        }
    }

    return boxpairFinalConnection;
}

// -----------------------------------------------------------
// Parts
// -----------------------------------------------------------

std::string Day08::Part1() {
    auto vectorCircuitSizes =
        CircuitSizesAfterConnections(m_vectorJunctionBoxes, m_vectorPairsByDistance, 1000);
    if (vectorCircuitSizes.size() < 3)
        return "0";

    std::int64_t iThreeLargestCircuitsProduct = std::int64_t(vectorCircuitSizes[0]) *
                                                std::int64_t(vectorCircuitSizes[1]) *
                                                std::int64_t(vectorCircuitSizes[2]);

    return std::to_string(iThreeLargestCircuitsProduct);
}

std::string Day08::Part2() {
    if (m_vectorJunctionBoxes.size() < 2)
        return "0";

    const auto boxpairFinalConnection =
        ConnectAllJunctionBoxes(m_vectorJunctionBoxes, m_vectorPairsByDistance);
    const auto iFirstX = m_vectorJunctionBoxes[boxpairFinalConnection.m_iFirstBoxIndex].m_iX;
    const auto iSecondX = m_vectorJunctionBoxes[boxpairFinalConnection.m_iSecondBoxIndex].m_iX;
    const auto magnitude_ = [](std::int64_t iSignedValue) {
        const auto uBits = static_cast<std::uint64_t>(iSignedValue);
        return iSignedValue < 0 ? std::uint64_t{0} - uBits : uBits;
    };
    const bool bProductIsNegative = (iFirstX < 0) != (iSecondX < 0);
    const auto uMaxMagnitude =
        static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + bProductIsNegative;
    const auto uFirstMagnitude = magnitude_(iFirstX), uSecondMagnitude = magnitude_(iSecondX);
    if (uSecondMagnitude != 0 && uFirstMagnitude > uMaxMagnitude / uSecondMagnitude)
        throw std::overflow_error("X coordinate product exceeds int64_t");
    const auto uProductMagnitude = uFirstMagnitude * uSecondMagnitude;
    if (bProductIsNegative && uProductMagnitude == uMaxMagnitude)
        return std::to_string(std::numeric_limits<std::int64_t>::min());
    const auto iSignedProduct = static_cast<std::int64_t>(uProductMagnitude);
    return std::to_string(bProductIsNegative ? -iSignedProduct : iSignedProduct);
}
