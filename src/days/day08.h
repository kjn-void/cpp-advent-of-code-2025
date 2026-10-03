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

    struct Pt {
        std::int64_t xJunction, yJunction, zJunction;
    };

    struct Edg {
        std::int64_t distSquared;
        int iptFirst, iptSecond;
    };

    std::vector<Pt> rgpt;
    std::vector<Edg> rgedgConnections;

    // Helpers
    static std::int64_t DistSquared(const Pt& ptFirst, const Pt& ptSecond);
    static std::vector<Edg> RgedgBuildSorted(std::span<const Pt> rgpt);

    // DSU
    struct Dsu {
        std::vector<int> mpiptiptParent;
        std::vector<int> mpiptcntSize;

        explicit Dsu(int cpt);
        int IptFind(int iptRoot);
        bool FUnite(int iptFirstRoot, int iptSecondRoot);
    };

    static std::vector<int> RgcntRunConnections(std::span<const Pt> rgpt,
                                                std::span<const Edg> rgedgConnections, int cedg);

    static std::pair<int, int> LinkConnectAll(std::span<const Pt> rgpt,
                                              std::span<const Edg> rgedgConnections);
};
