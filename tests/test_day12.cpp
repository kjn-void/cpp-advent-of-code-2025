#include <gtest/gtest.h>
#include "days/day12.h"
#include "test_utils.h"

extern std::vector<std::string> splitLines(const std::string&);

static const char* example = R"(
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
    Day12 d;
    d.set_input(splitLines(example));
    EXPECT_EQ(d.part1(), "2");
}