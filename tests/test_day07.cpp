#include <gtest/gtest.h>

#include "days/day07.h"

// ------------------------
// Example test data
// ------------------------

static const std::vector<std::string> day07_example_input = {
    ".......S.......",
    "...............",
    ".......^.......",
    "...............",
    "......^.^......",
    "...............",
    ".....^.^.^.....",
    "...............",
    "....^.^...^....",
    "...............",
    "...^.^...^.^...",
    "...............",
    "..^...^.....^..",
    "...............",
    ".^.^.^.^.^...^.",
    "...............",
};

// ------------------------
// Unit tests
// ------------------------

TEST(Day07, ExamplePart1) {
    Day07 d;
    d.set_input(day07_example_input);

    EXPECT_EQ(d.part1(), "21");
}

TEST(Day07, ExamplePart2) {
    Day07 d;
    d.set_input(day07_example_input);

    EXPECT_EQ(d.part2(), "40");
}