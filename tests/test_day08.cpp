#include <gtest/gtest.h>
#include "days/day08.h"

static const std::vector<std::string> example_day08 = {
    "162,817,812",
    "57,618,57",
    "906,360,560",
    "592,479,940",
    "352,342,300",
    "466,668,158",
    "542,29,236",
    "431,825,988",
    "739,650,466",
    "52,470,668",
    "216,146,977",
    "819,987,18",
    "117,168,530",
    "805,96,715",
    "346,949,466",
    "970,615,88",
    "941,993,340",
    "862,61,35",
    "984,92,344",
    "425,690,689",
};

TEST(Day08, ExamplePart1) {
    Day08 d;
    d.set_input(example_day08);

    // Example uses 10 shortest connections
    auto sizes = Day08::run_connections(d.points, d.edges, 10);
    ASSERT_GE(sizes.size(), 3);
    EXPECT_EQ(sizes[0] * sizes[1] * sizes[2], 40);
}

TEST(Day08, ExamplePart2) {
    Day08 d;
    d.set_input(example_day08);

    auto [i, j] = Day08::run_until_single_circuit(d.points, d.edges);
    EXPECT_EQ(d.points[i].x * d.points[j].x, 25272);
}