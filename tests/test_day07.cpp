#include <gtest/gtest.h>

#include "days/day07.h"

// ------------------------
// Example test data
// ------------------------

static const std::vector<std::string> vectorExampleInput = {
    ".......S.......", "...............", ".......^.......", "...............",
    "......^.^......", "...............", ".....^.^.^.....", "...............",
    "....^.^...^....", "...............", "...^.^...^.^...", "...............",
    "..^...^.....^..", "...............", ".^.^.^.^.^...^.", "...............",
};

// ------------------------
// Unit tests
// ------------------------

TEST(Day07, ExamplePart1) {
    Day07 day07Solver;
    day07Solver.SetInput(vectorExampleInput);

    EXPECT_EQ(day07Solver.Part1(), "21");
}

TEST(Day07, ExamplePart2) {
    Day07 day07Solver;
    day07Solver.SetInput(vectorExampleInput);

    EXPECT_EQ(day07Solver.Part2(), "40");
}