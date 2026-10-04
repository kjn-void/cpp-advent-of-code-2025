#include "days/day03.h"
#include <gtest/gtest.h>

TEST(Day03, ExamplePart1) {
    Day03 day03Solver;
    day03Solver.SetInput({
        "987654321111111",
        "811111111111119",
        "234234234234278",
        "818181911112111",
    });

    EXPECT_EQ(day03Solver.Part1(), "357");
}

TEST(Day03, ExamplePart2) {
    Day03 day03Solver;
    day03Solver.SetInput({
        "987654321111111",
        "811111111111119",
        "234234234234278",
        "818181911112111",
    });

    EXPECT_EQ(day03Solver.Part2(), "3121910778619");
}