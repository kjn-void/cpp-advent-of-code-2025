#include "days/day09.h"
#include <gtest/gtest.h>

static const std::vector<std::string> example_input = {
    "7,1", "11,1", "11,7", "9,7", "9,5", "2,5", "2,3", "7,3",
};

TEST(Day09, ExamplePart1) {
    Day09 solver;
    solver.set_input(example_input);
    EXPECT_EQ(solver.part1(), "50");
}

TEST(Day09, ExamplePart2) {
    Day09 solver;
    solver.set_input(example_input);
    EXPECT_EQ(solver.part2(), "24");
}