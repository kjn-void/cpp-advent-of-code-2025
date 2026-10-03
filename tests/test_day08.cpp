#include "days/day08.h"
#include <gtest/gtest.h>

static const std::vector<std::string> rgusDay08Sample = {
    "162,817,812", "57,618,57",   "906,360,560", "592,479,940", "352,342,300",
    "466,668,158", "542,29,236",  "431,825,988", "739,650,466", "52,470,668",
    "216,146,977", "819,987,18",  "117,168,530", "805,96,715",  "346,949,466",
    "970,615,88",  "941,993,340", "862,61,35",   "984,92,344",  "425,690,689",
};

TEST(Day08, ExamplePart1) {
    Day08 slvDay;
    slvDay.SetInput(rgusDay08Sample);

    // Example uses 10 shortest connections
    auto rgcntCircuitSizes = Day08::RgcntRunConnections(slvDay.rgpt, slvDay.rgedgConnections, 10);
    ASSERT_GE(rgcntCircuitSizes.size(), 3);
    EXPECT_EQ(rgcntCircuitSizes[0] * rgcntCircuitSizes[1] * rgcntCircuitSizes[2], 40);
}

TEST(Day08, ExamplePart2) {
    Day08 slvDay;
    slvDay.SetInput(rgusDay08Sample);

    auto [iptFirst, iptSecond] = Day08::LinkConnectAll(slvDay.rgpt, slvDay.rgedgConnections);
    EXPECT_EQ(slvDay.rgpt[iptFirst].xJunction * slvDay.rgpt[iptSecond].xJunction, 25272);
}