#include "days/day12.h"
#include "test_utils.h"
#include <gtest/gtest.h>

static const char* example_text = R"(
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
    Day12 solver;
    solver.set_input(split_lines(example_text));
    EXPECT_EQ(solver.part1(), "2");
}