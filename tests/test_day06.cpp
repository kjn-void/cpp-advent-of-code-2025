#include "days/day06.h"
#include <gtest/gtest.h>

static std::vector<std::string> vectorExampleInput = {
    "123 328  51 64 ",
    " 45 64  387 23 ",
    "  6 98  215 314",
    "*   +   *   +  ",
};

TEST(Day06, ExamplePart1) {
    Day06 day06Solver;
    day06Solver.SetInput(vectorExampleInput);
    EXPECT_EQ(day06Solver.Part1(), "4277556");
}

TEST(Day06, ExamplePart2) {
    Day06 day06Solver;
    day06Solver.SetInput(vectorExampleInput);
    EXPECT_EQ(day06Solver.Part2(), "3263827");
}