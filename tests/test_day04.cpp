#include "days/day04.h"
#include <gtest/gtest.h>

static std::vector<std::string> example_input() {
    return {"..@@.@@@@.", "@@@.@.@.@@", "@@@@@.@.@@", "@.@@@@..@.", "@@.@@@@.@@",
            ".@@@@@@@.@", ".@.@.@.@@@", "@.@@@.@@@@", ".@@@@@@@@.", "@.@.@@@.@."};
}

TEST(Day04, ExamplePart1) {
    Day04 solver;
    solver.set_input(example_input());
    EXPECT_EQ(solver.part1(), "13");
}

TEST(Day04, ExamplePart2) {
    Day04 solver;
    solver.set_input(example_input());
    EXPECT_EQ(solver.part2(), "43");
}