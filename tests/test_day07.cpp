#include <gtest/gtest.h>

#include "days/day07.h"

// ------------------------
// Example test data
// ------------------------

static const std::vector<std::string> rgusDay07Sample = {
    ".......S.......", "...............", ".......^.......", "...............",
    "......^.^......", "...............", ".....^.^.^.....", "...............",
    "....^.^...^....", "...............", "...^.^...^.^...", "...............",
    "..^...^.....^..", "...............", ".^.^.^.^.^...^.", "...............",
};

// ------------------------
// Unit tests
// ------------------------

TEST(Day07, ExamplePart1) {
    Day07 slvDay;
    slvDay.SetInput(rgusDay07Sample);

    EXPECT_EQ(slvDay.TxtPart1(), "21");
}

TEST(Day07, ExamplePart2) {
    Day07 slvDay;
    slvDay.SetInput(rgusDay07Sample);

    EXPECT_EQ(slvDay.TxtPart2(), "40");
}