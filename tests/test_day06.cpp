#include "days/day06.h"
#include <gtest/gtest.h>

static std::vector<std::string> example_input = {
    "123 328  51 64 ",
    " 45 64  387 23 ",
    "  6 98  215 314",
    "*   +   *   +  ",
};

TEST(Day06, ExamplePart1) {
    Day06 solver;
    solver.set_input(example_input);
    EXPECT_EQ(solver.part1(), "4277556");
}

TEST(Day06, ExamplePart2) {
    Day06 solver;
    solver.set_input(example_input);
    EXPECT_EQ(solver.part2(), "3263827");
}