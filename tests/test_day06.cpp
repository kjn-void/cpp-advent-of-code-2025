#include <gtest/gtest.h>
#include "days/day06.h"

static std::vector<std::string> example = {
    "123 328  51 64 ",
    " 45 64  387 23 ",
    "  6 98  215 314",
    "*   +   *   +  ",
};

TEST(Day06, ExamplePart1) {
    Day06 d;
    d.set_input(example);
    EXPECT_EQ(d.part1(), "4277556");
}

TEST(Day06, ExamplePart2) {
    Day06 d;
    d.set_input(example);
    EXPECT_EQ(d.part2(), "3263827");
}