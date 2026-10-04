#include "days/day04.h"
#include <gtest/gtest.h>

static std::vector<std::string> ExampleInput() {
    return {"..@@.@@@@.", "@@@.@.@.@@", "@@@@@.@.@@", "@.@@@@..@.", "@@.@@@@.@@",
            ".@@@@@@@.@", ".@.@.@.@@@", "@.@@@.@@@@", ".@@@@@@@@.", "@.@.@@@.@."};
}

TEST(Day04, ExamplePart1) {
    Day04 day04Solver;
    day04Solver.SetInput(ExampleInput());
    EXPECT_EQ(day04Solver.Part1(), "13");
}

TEST(Day04, ExamplePart2) {
    Day04 day04Solver;
    day04Solver.SetInput(ExampleInput());
    EXPECT_EQ(day04Solver.Part2(), "43");
}