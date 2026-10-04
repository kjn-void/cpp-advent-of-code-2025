#include <gtest/gtest.h>

#include "days/day07.h"

// ------------------------
// Example test data
// ------------------------

static const std::vector<std::string> example_input = {
    ".......S.......", "...............", ".......^.......", "...............",
    "......^.^......", "...............", ".....^.^.^.....", "...............",
    "....^.^...^....", "...............", "...^.^...^.^...", "...............",
    "..^...^.....^..", "...............", ".^.^.^.^.^...^.", "...............",
};

// ------------------------
// Unit tests
// ------------------------

TEST(Day07, ExamplePart1) {
    Day07 solver;
    solver.set_input(example_input);

    EXPECT_EQ(solver.part1(), "21");
}

TEST(Day07, ExamplePart2) {
    Day07 solver;
    solver.set_input(example_input);

    EXPECT_EQ(solver.part2(), "40");
}