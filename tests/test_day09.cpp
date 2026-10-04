#include "days/day09.h"
#include <gtest/gtest.h>

static const std::vector<std::string> vectorExampleInput = {
    "7,1", "11,1", "11,7", "9,7", "9,5", "2,5", "2,3", "7,3",
};

TEST(Day09, ExamplePart1) {
    Day09 day09Solver;
    day09Solver.SetInput(vectorExampleInput);
    EXPECT_EQ(day09Solver.Part1(), "50");
}

TEST(Day09, ExamplePart2) {
    Day09 day09Solver;
    day09Solver.SetInput(vectorExampleInput);
    EXPECT_EQ(day09Solver.Part2(), "24");
}