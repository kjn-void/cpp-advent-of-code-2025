#include <gtest/gtest.h>
#include "days/day03.h"

TEST(Day03, ExamplePart1) {
    Day03 d;
    d.set_input({
        "987654321111111",
        "811111111111119",
        "234234234234278",
        "818181911112111",
    });

    EXPECT_EQ(d.part1(), "357");
}

TEST(Day03, ExamplePart2) {
    Day03 d;
    d.set_input({
        "987654321111111",
        "811111111111119",
        "234234234234278",
        "818181911112111",
    });

    EXPECT_EQ(d.part2(), "3121910778619");
}