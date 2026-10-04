#pragma once

#include <cstdint>

#include "core/Solution.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Day11 final : public Solution {
  public:
    void SetInput(const std::vector<std::string>& vectorInputLines) override;
    std::string Part1() override;
    std::string Part2() override;

  private:
    // adjacency list
    std::unordered_map<std::string, std::vector<std::string>> m_mapOutputsByDevice;

    // ---------- Part 1 ----------
    std::int64_t
    CountPathsFrom(const std::string& stringDevice,
                   std::unordered_map<std::string, std::int64_t>& mapPathCountsByDevice,
                   std::unordered_set<std::string>& setDevicesOnPath);

    // ---------- Part 2 ----------
    struct VisitState {
        std::string m_stringDevice;
        int m_iRequiredVisitMask;

        bool operator==(const VisitState& visitstateOther) const {
            return m_stringDevice == visitstateOther.m_stringDevice &&
                   m_iRequiredVisitMask == visitstateOther.m_iRequiredVisitMask;
        }
    };

    struct VisitStateHash {
        std::size_t operator()(const VisitState& visitstate) const {
            return std::hash<std::string>()(visitstate.m_stringDevice) ^
                   (std::hash<int>()(visitstate.m_iRequiredVisitMask) << 1);
        }
    };

    std::int64_t CountPathsThroughRequiredDevices(const std::string& stringStartDevice,
                                                  const std::string& stringEndDevice,
                                                  const std::string& stringFirstRequiredDevice,
                                                  const std::string& stringSecondRequiredDevice);
};
