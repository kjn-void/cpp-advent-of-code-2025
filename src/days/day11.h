#pragma once

#include <cstdint>

#include "core/Solution.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Day11 final : public Slv {
  public:
    void SetInput(const std::vector<std::string>& rgusLines) override;
    std::string TxtPart1() override;
    std::string TxtPart2() override;

  private:
    // adjacency list
    std::unordered_map<std::string, std::vector<std::string>> mpdevrgdevOutputs_;

    // ---------- Part 1 ----------
    std::int64_t CntPathsFrom(const std::string& dev,
                              std::unordered_map<std::string, std::int64_t>& mpdevcntPaths,
                              std::unordered_set<std::string>& setdevOnPath);

    // ---------- Part 2 ----------
    struct Vst {
        std::string dev;
        int maskVisits;

        bool operator==(const Vst& vstOther) const {
            return dev == vstOther.dev && maskVisits == vstOther.maskVisits;
        }
    };

    struct Hashvst {
        std::size_t operator()(const Vst& vst) const {
            return std::hash<std::string>()(vst.dev) ^ (std::hash<int>()(vst.maskVisits) << 1);
        }
    };

    std::int64_t CntPathsThroughRequired(const std::string& devStart, const std::string& devEnd,
                                         const std::string& devRequiredFirst,
                                         const std::string& devRequiredSecond);
};
