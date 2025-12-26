#include <gtest/gtest.h>
#include "days/day04.h"

static std::vector<std::string> example_input() {
    return {
        "..@@.@@@@.",
        "@@@.@.@.@@",
        "@@@@@.@.@@",
        "@.@@@@..@.",
        "@@.@@@@.@@",
        ".@@@@@@@.@",
        ".@.@.@.@@@",
        "@.@@@.@@@@",
        ".@@@@@@@@.",
        "@.@.@@@.@."
    };
}

TEST(Day04, ExamplePart1) {
    Day04 d;
    d.set_input(example_input());
    EXPECT_EQ(d.part1(), "13");
}

TEST(Day04, ExamplePart2) {
    Day04 d;
    d.set_input(example_input());
    EXPECT_EQ(d.part2(), "43");
}