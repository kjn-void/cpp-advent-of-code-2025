#include <gtest/gtest.h>
#include "days/day05.h"

static std::vector<std::string> example_input() {
    return {
        "3-5",
        "10-14",
        "16-20",
        "12-18",
        "",
        "1",
        "5",
        "8",
        "11",
        "17",
        "32"
    };
}

TEST(Day05, ExamplePart1) {
    Day05 d;
    d.set_input(example_input());
    EXPECT_EQ(d.part1(), "3");
}

TEST(Day05, ExamplePart2) {
    Day05 d;
    d.set_input(example_input());
    EXPECT_EQ(d.part2(), "14");
}