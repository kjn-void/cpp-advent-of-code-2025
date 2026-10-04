#include "days/day11.h"
#include "test_utils.h"
#include <gtest/gtest.h>

static const char* part1_example = R"(
aaa: you hhh
you: bbb ccc
bbb: ddd eee
ccc: ddd eee fff
ddd: ggg
eee: out
fff: out
ggg: out
hhh: ccc fff iii
iii: out
)";

static const char* part2_example = R"(
svr: aaa bbb
aaa: fft
fft: ccc
bbb: tty
tty: ccc
ccc: ddd eee
ddd: hub
hub: fff
eee: dac
dac: fff
fff: ggg hhh
ggg: out
hhh: out
)";

TEST(Day11, ExamplePart1) {
    Day11 solver;
    solver.set_input(split_lines(part1_example));
    EXPECT_EQ(solver.part1(), "5");
}

TEST(Day11, ExamplePart2) {
    Day11 solver;
    solver.set_input(split_lines(part2_example));
    EXPECT_EQ(solver.part2(), "2");
}