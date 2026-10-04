#include "days/day11.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <sstream>
#include <stdexcept>
#include <unordered_set>

// Registration
namespace {
const core::DayRegistration<Day11> dayregistration{11};

constexpr int iFirstRequiredVisitBit = 1;
constexpr int iSecondRequiredVisitBit = 2;
constexpr int iAllRequiredVisitsMask = iFirstRequiredVisitBit | iSecondRequiredVisitBit;
} // namespace

// ------------------------------------------------------------
// Parsing
// ------------------------------------------------------------

void Day11::SetInput(const std::vector<std::string>& vectorInputLines) {
    m_mapOutputsByDevice.clear();

    for (const auto& stringLine : vectorInputLines) {
        if (stringLine.empty())
            continue;

        auto uColonOffset = stringLine.find(':');
        if (uColonOffset == std::string::npos)
            throw std::invalid_argument("Expected a device and its outputs");

        std::string stringSourceDevice(
            core::Trim(std::string_view(stringLine).substr(0, uColonOffset)));
        if (stringSourceDevice.empty())
            throw std::invalid_argument("Missing device name");
        std::string stringOutputNames = stringLine.substr(uColonOffset + 1);

        std::istringstream istringstreamOutputs(stringOutputNames);
        std::string stringDeviceName;
        std::vector<std::string> vectorOutputDevices;

        while (istringstreamOutputs >> stringDeviceName) {
            vectorOutputDevices.push_back(stringDeviceName);
        }

        m_mapOutputsByDevice[stringSourceDevice] = std::move(vectorOutputDevices);
    }
}

// ------------------------------------------------------------
// Part 1 — count all paths from "you" to "out"
// ------------------------------------------------------------

std::int64_t
Day11::CountPathsFrom(const std::string& stringDevice,
                      std::unordered_map<std::string, std::int64_t>& mapPathCountsByDevice,
                      std::unordered_set<std::string>& setDevicesOnPath) {
    if (stringDevice == "out") {
        return 1;
    }

    if (auto itFound = mapPathCountsByDevice.find(stringDevice);
        itFound != mapPathCountsByDevice.end()) {
        return itFound->second;
    }

    // cycle guard (should not happen for valid input)
    if (!setDevicesOnPath.insert(stringDevice).second) {
        throw std::invalid_argument("Device graph contains a cycle");
    }

    std::int64_t iPathCount = 0;
    if (const auto itFound = m_mapOutputsByDevice.find(stringDevice);
        itFound != m_mapOutputsByDevice.end()) {
        for (const auto& stringNextDevice : itFound->second)
            iPathCount += CountPathsFrom(stringNextDevice, mapPathCountsByDevice, setDevicesOnPath);
    }

    setDevicesOnPath.erase(stringDevice);
    mapPathCountsByDevice[stringDevice] = iPathCount;
    return iPathCount;
}

std::string Day11::Part1() {
    if (m_mapOutputsByDevice.empty()) {
        return "0";
    }

    std::unordered_map<std::string, std::int64_t> mapPathCountsByDevice;
    std::unordered_set<std::string> setDevicesOnPath;

    std::int64_t iPathCount = CountPathsFrom("you", mapPathCountsByDevice, setDevicesOnPath);
    return std::to_string(iPathCount);
}

// ------------------------------------------------------------
// Part 2 — paths from "svr" to "out" that visit both "dac" and "fft"
// ------------------------------------------------------------

std::int64_t Day11::CountPathsThroughRequiredDevices(
    const std::string& stringStartDevice, const std::string& stringEndDevice,
    const std::string& stringFirstRequiredDevice, const std::string& stringSecondRequiredDevice) {
    if (m_mapOutputsByDevice.empty()) {
        return 0;
    }

    std::unordered_map<VisitState, std::int64_t, VisitStateHash> mapPathCountsByState;
    std::unordered_set<VisitState, VisitStateHash> setStatesOnPath;

    int iInitialVisitMask = 0;
    if (stringStartDevice == stringFirstRequiredDevice)
        iInitialVisitMask |= iFirstRequiredVisitBit;
    if (stringStartDevice == stringSecondRequiredDevice)
        iInitialVisitMask |= iSecondRequiredVisitBit;

    const auto count_paths_ = [&](auto&& recurse_, const std::string& stringDevice,
                                  int iRequiredVisitMask) -> std::int64_t {
        VisitState visitstate{stringDevice, iRequiredVisitMask};

        if (auto itFound = mapPathCountsByState.find(visitstate);
            itFound != mapPathCountsByState.end()) {
            return itFound->second;
        }

        if (stringDevice == stringEndDevice) {
            return mapPathCountsByState[visitstate] =
                       (iRequiredVisitMask == iAllRequiredVisitsMask ? 1 : 0);
        }

        if (!setStatesOnPath.insert(visitstate).second)
            throw std::invalid_argument("Device graph contains a cycle");
        std::int64_t iPathCount = 0;
        if (const auto itFound = m_mapOutputsByDevice.find(stringDevice);
            itFound != m_mapOutputsByDevice.end()) {
            for (const auto& stringNextDevice : itFound->second) {
                int iNextVisitMask = iRequiredVisitMask;
                if (stringNextDevice == stringFirstRequiredDevice)
                    iNextVisitMask |= iFirstRequiredVisitBit;
                if (stringNextDevice == stringSecondRequiredDevice)
                    iNextVisitMask |= iSecondRequiredVisitBit;
                iPathCount += recurse_(recurse_, stringNextDevice, iNextVisitMask);
            }
        }
        setStatesOnPath.erase(visitstate);
        mapPathCountsByState[visitstate] = iPathCount;
        return iPathCount;
    };

    return count_paths_(count_paths_, stringStartDevice, iInitialVisitMask);
}

std::string Day11::Part2() {
    std::int64_t iPathCount = CountPathsThroughRequiredDevices("svr", "out", "dac", "fft");
    return std::to_string(iPathCount);
}
