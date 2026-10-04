#include "days/day03.h"
#include <gtest/gtest.h>

TEST(Day03, ExamplePart1) {
    Day03 solver;
    solver.set_input({
        "987654321111111",
        "811111111111119",
        "234234234234278",
        "818181911112111",
    });

    EXPECT_EQ(solver.part1(), "357");
}

TEST(Day03, ExamplePart2) {
    Day03 solver;
    solver.set_input({
        "987654321111111",
        "811111111111119",
        "234234234234278",
        "818181911112111",
    });

    EXPECT_EQ(solver.part2(), "3121910778619");
}