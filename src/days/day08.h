#pragma once

#include <span>

#include "core/Solution.h"

#include <cstdint>
#include <string>
#include <vector>

class Day08 final : public Solution {
  public:
    void SetInput(const std::vector<std::string>& vectorInputLines) override;
    std::string Part1() override;
    std::string Part2() override;

    struct JunctionBox {
        std::int64_t m_iX, m_iY, m_iZ;
    };

    // Every pair of junction boxes, a candidate for connection.
    struct BoxPair {
        std::int64_t m_iSquaredDistance;
        int m_iFirstBoxIndex, m_iSecondBoxIndex;
    };

    std::vector<JunctionBox> m_vectorJunctionBoxes;
    std::vector<BoxPair> m_vectorPairsByDistance;

    // Helpers
    static std::int64_t SquaredDistance(const JunctionBox& junctionboxFirst,
                                        const JunctionBox& junctionboxSecond);
    static std::vector<BoxPair> SortPairsByDistance(std::span<const JunctionBox> spanJunctionBoxes);

    // Union-find
    struct CircuitSet {
        std::vector<int> m_vectorParentByBox;
        std::vector<int> m_vectorBoxCountByRoot;

        explicit CircuitSet(int iBoxCount);
        int FindCircuitRoot(int iBoxIndex);
        bool JoinCircuits(int iFirstBoxIndex, int iSecondBoxIndex);
    };

    static std::vector<int>
    CircuitSizesAfterConnections(std::span<const JunctionBox> spanJunctionBoxes,
                                 std::span<const BoxPair> spanPairsByDistance,
                                 int iConnectionCount);

    static BoxPair ConnectAllJunctionBoxes(std::span<const JunctionBox> spanJunctionBoxes,
                                           std::span<const BoxPair> spanPairsByDistance);
};
