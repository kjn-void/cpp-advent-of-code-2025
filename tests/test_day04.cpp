#include "days/day04.h"
#include <gtest/gtest.h>

static std::vector<std::string> RgusExampleInput() {
    return {"..@@.@@@@.", "@@@.@.@.@@", "@@@@@.@.@@", "@.@@@@..@.", "@@.@@@@.@@",
            ".@@@@@@@.@", ".@.@.@.@@@", "@.@@@.@@@@", ".@@@@@@@@.", "@.@.@@@.@."};
}

TEST(Day04, ExamplePart1) {
    Day04 slvDay;
    slvDay.SetInput(RgusExampleInput());
    EXPECT_EQ(slvDay.TxtPart1(), "13");
}

TEST(Day04, ExamplePart2) {
    Day04 slvDay;
    slvDay.SetInput(RgusExampleInput());
    EXPECT_EQ(slvDay.TxtPart2(), "43");
}