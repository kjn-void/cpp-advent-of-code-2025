#pragma once

#include <span>

#include <utility>

#include "core/Solution.h"

#include <cstdint>
#include <string>
#include <vector>

class Day08 final : public Slv {
  public:
    void SetInput(const std::vector<std::string>& rgusLines) override;
    std::string TxtPart1() override;
    std::string TxtPart2() override;

    struct Jb {
        std::int64_t x, y, z;
    };

    struct Cn {
        std::int64_t distSquared;
        int ijbFirst, ijbSecond;
    };

    std::vector<Jb> rgjb;
    std::vector<Cn> rgcn;

    // Helpers
    static std::int64_t DistSquared(const Jb& jbFirst, const Jb& jbSecond);
    static std::vector<Cn> RgcnBuildSorted(std::span<const Jb> rgjb);

    // DSU
    struct Dsu {
        std::vector<int> mpijbijbParent;
        std::vector<int> mpijbcjbSize;

        explicit Dsu(int cjb);
        int IjbFindCircuit(int ijbRoot);
        bool FUnite(int ijbFirstRoot, int ijbSecondRoot);
    };

    static std::vector<int> RgcjbConnectNearest(std::span<const Jb> rgjb, std::span<const Cn> rgcn,
                                                int ccn);

    static std::pair<int, int> LinkConnectAll(std::span<const Jb> rgjb, std::span<const Cn> rgcn);
};
