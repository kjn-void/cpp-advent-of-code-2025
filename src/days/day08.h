#pragma once

#include <span>

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

    // Every pair of junction boxes is a candidate connection.
    struct Cn {
        std::int64_t distSquared;
        int ijbFirst, ijbSecond;
    };

    std::vector<Jb> rgjb;
    std::vector<Cn> rgcnByDistance;

    // Helpers
    static std::int64_t DistSquared(const Jb& jbFirst, const Jb& jbSecond);
    static std::vector<Cn> RgcnSortByDistance(std::span<const Jb> rgjb);

    // Union-find
    struct Dsu {
        std::vector<int> mpijbijbParent;
        std::vector<int> mpijbcjbSize;

        explicit Dsu(int cjb);
        int IjbFindRoot(int ijb);
        bool FUnite(int ijbFirst, int ijbSecond);
    };

    static std::vector<int> RgcjbConnectNearest(std::span<const Jb> rgjb,
                                                std::span<const Cn> rgcnByDistance, int ccn);

    static Cn CnConnectAll(std::span<const Jb> rgjb, std::span<const Cn> rgcnByDistance);
};
