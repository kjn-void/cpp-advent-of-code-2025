#include "days/day08.h"
#include <gtest/gtest.h>

static const std::vector<std::string> example_input = {
    "162,817,812", "57,618,57",   "906,360,560", "592,479,940", "352,342,300",
    "466,668,158", "542,29,236",  "431,825,988", "739,650,466", "52,470,668",
    "216,146,977", "819,987,18",  "117,168,530", "805,96,715",  "346,949,466",
    "970,615,88",  "941,993,340", "862,61,35",   "984,92,344",  "425,690,689",
};

TEST(Day08, ExamplePart1) {
    Day08 solver;
    solver.set_input(example_input);

    // Example uses 10 shortest connections
    auto circuit_sizes =
        Day08::circuit_sizes_after_connections(solver.junction_boxes, solver.connections, 10);
    ASSERT_GE(circuit_sizes.size(), 3);
    EXPECT_EQ(circuit_sizes[0] * circuit_sizes[1] * circuit_sizes[2], 40);
}

TEST(Day08, ExamplePart2) {
    Day08 solver;
    solver.set_input(example_input);

    auto [first_box_index, second_box_index] =
        Day08::connect_all_junction_boxes(solver.junction_boxes, solver.connections);
    EXPECT_EQ(solver.junction_boxes[first_box_index].x * solver.junction_boxes[second_box_index].x,
              25272);
}