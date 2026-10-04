#include "days/day10.h"
#include "test_utils.h"
#include <gtest/gtest.h>

static const char* pbszExampleText = R"(
[.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
[...#.] (0,2,3,4) (2,3) (0,4) (0,1,2) (1,2,3,4) {7,5,12,7,2}
[.###.#] (0,1,2,3,4) (0,3,4) (0,1,2,4,5) (1,2) {10,11,11,5,10,5}
)";

TEST(Day10, ExamplePart1) {
    Day10 day10Solver;
    day10Solver.SetInput(SplitLines(pbszExampleText));
    EXPECT_EQ(day10Solver.Part1(), "7");
}

TEST(Day10, ExamplePart2) {
    Day10 day10Solver;
    day10Solver.SetInput(SplitLines(pbszExampleText));
    EXPECT_EQ(day10Solver.Part2(), "33");
}