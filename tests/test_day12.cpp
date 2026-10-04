#include "days/day12.h"
#include "test_utils.h"
#include <gtest/gtest.h>

static const char* pbszExampleText = R"(
0:
###
##.
##.

1:
###
##.
.##

2:
.##
###
##.

3:
##.
###
##.

4:
###
#..
###

5:
###
.#.
###

4x4: 0 0 0 0 2 0
12x5: 1 0 1 0 2 2
12x5: 1 0 1 0 3 2
)";

TEST(Day12, ExamplePart1) {
    Day12 day12Solver;
    day12Solver.SetInput(SplitLines(pbszExampleText));
    EXPECT_EQ(day12Solver.Part1(), "2");
}