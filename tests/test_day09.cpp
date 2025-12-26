#include <gtest/gtest.h>
#include "days/day09.h"

static const std::vector<std::string> exampleDay09 = {
    "7,1",
    "11,1",
    "11,7",
    "9,7",
    "9,5",
    "2,5",
    "2,3",
    "7,3",
};

TEST(Day09, ExamplePart1) {
    Day09 d;
    d.set_input(exampleDay09);
    EXPECT_EQ(d.part1(), "50");
}

TEST(Day09, ExamplePart2) {
    Day09 d;
    d.set_input(exampleDay09);
    EXPECT_EQ(d.part2(), "24");
}