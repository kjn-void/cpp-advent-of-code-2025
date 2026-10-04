#include "days/day11.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <sstream>
#include <stdexcept>
#include <unordered_set>

// Registration
namespace {
const core::Drg<Day11> drgDay{11};

constexpr int maskFirstRequired = 1;
constexpr int maskSecondRequired = 2;
constexpr int maskAllRequired = maskFirstRequired | maskSecondRequired;
} // namespace

// ------------------------------------------------------------
// Parsing
// ------------------------------------------------------------

void Day11::SetInput(const std::vector<std::string>& rgusLines) {
    mpdevrgdevOutputs_.clear();

    for (const auto& usLine : rgusLines) {
        if (usLine.empty())
            continue;

        auto offColon = usLine.find(':');
        if (offColon == std::string::npos)
            throw std::invalid_argument("Expected a device and its outputs");

        std::string devSource(core::UsTrim(std::string_view(usLine).substr(0, offColon)));
        if (devSource.empty())
            throw std::invalid_argument("Missing device name");
        std::string usOutputs = usLine.substr(offColon + 1);

        std::istringstream inOutputs(usOutputs);
        std::string devOutput;
        std::vector<std::string> rgdevOutputs;

        while (inOutputs >> devOutput) {
            rgdevOutputs.push_back(devOutput);
        }

        mpdevrgdevOutputs_[devSource] = std::move(rgdevOutputs);
    }
}

// ------------------------------------------------------------
// Part 1 — count all paths from "you" to "out"
// ------------------------------------------------------------

std::int64_t Day11::CntPathsFrom(const std::string& dev,
                                 std::unordered_map<std::string, std::int64_t>& mpdevcntPaths,
                                 std::unordered_set<std::string>& setdevOnPath) {
    if (dev == "out") {
        return 1;
    }

    if (auto itLookup = mpdevcntPaths.find(dev); itLookup != mpdevcntPaths.end()) {
        return itLookup->second;
    }

    // cycle guard (should not happen for valid input)
    if (!setdevOnPath.insert(dev).second) {
        throw std::invalid_argument("Device graph contains a cycle");
    }

    std::int64_t cntPaths = 0;
    if (const auto itLookup = mpdevrgdevOutputs_.find(dev); itLookup != mpdevrgdevOutputs_.end()) {
        for (const auto& devNext : itLookup->second)
            cntPaths += CntPathsFrom(devNext, mpdevcntPaths, setdevOnPath);
    }

    setdevOnPath.erase(dev);
    mpdevcntPaths[dev] = cntPaths;
    return cntPaths;
}

std::string Day11::TxtPart1() {
    if (mpdevrgdevOutputs_.empty()) {
        return "0";
    }

    std::unordered_map<std::string, std::int64_t> mpdevcntPaths;
    std::unordered_set<std::string> setdevOnPath;

    std::int64_t cntPaths = CntPathsFrom("you", mpdevcntPaths, setdevOnPath);
    return std::to_string(cntPaths);
}

// ------------------------------------------------------------
// Part 2 — paths from "svr" to "out" that visit both "dac" and "fft"
// ------------------------------------------------------------

std::int64_t Day11::CntPathsThroughRequired(const std::string& devStart, const std::string& devEnd,
                                            const std::string& devRequiredFirst,
                                            const std::string& devRequiredSecond) {
    if (mpdevrgdevOutputs_.empty()) {
        return 0;
    }

    std::unordered_map<Vst, std::int64_t, Hashvst> mpvstcntPaths;
    std::unordered_set<Vst, Hashvst> setvstOnPath;

    int maskInitialVisits = 0;
    if (devStart == devRequiredFirst)
        maskInitialVisits |= maskFirstRequired;
    if (devStart == devRequiredSecond)
        maskInitialVisits |= maskSecondRequired;

    const auto fnCountPaths = [&](auto&& fnRecurCountPaths, const std::string& dev,
                                  int maskVisits) -> std::int64_t {
        Vst vst{dev, maskVisits};

        if (auto itLookup = mpvstcntPaths.find(vst); itLookup != mpvstcntPaths.end()) {
            return itLookup->second;
        }

        if (dev == devEnd) {
            return mpvstcntPaths[vst] = (maskVisits == maskAllRequired ? 1 : 0);
        }

        if (!setvstOnPath.insert(vst).second)
            throw std::invalid_argument("Device graph contains a cycle");
        std::int64_t cntPaths = 0;
        if (const auto itLookup = mpdevrgdevOutputs_.find(dev);
            itLookup != mpdevrgdevOutputs_.end()) {
            for (const auto& devNext : itLookup->second) {
                int maskNextVisits = maskVisits;
                if (devNext == devRequiredFirst)
                    maskNextVisits |= maskFirstRequired;
                if (devNext == devRequiredSecond)
                    maskNextVisits |= maskSecondRequired;
                cntPaths += fnRecurCountPaths(fnRecurCountPaths, devNext, maskNextVisits);
            }
        }
        setvstOnPath.erase(vst);
        mpvstcntPaths[vst] = cntPaths;
        return cntPaths;
    };

    return fnCountPaths(fnCountPaths, devStart, maskInitialVisits);
}

std::string Day11::TxtPart2() {
    std::int64_t cntPaths = CntPathsThroughRequired("svr", "out", "dac", "fft");
    return std::to_string(cntPaths);
}
