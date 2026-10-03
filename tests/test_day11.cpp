#include "days/day11.h"
#include "test_utils.h"
#include <gtest/gtest.h>

static const char* usPart1Sample = R"(
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

static const char* usPart2Sample = R"(
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
    Day11 slvDay;
    slvDay.SetInput(RgusSplitLines(usPart1Sample));
    EXPECT_EQ(slvDay.TxtPart1(), "5");
}

TEST(Day11, ExamplePart2) {
    Day11 slvDay;
    slvDay.SetInput(RgusSplitLines(usPart2Sample));
    EXPECT_EQ(slvDay.TxtPart2(), "2");
}