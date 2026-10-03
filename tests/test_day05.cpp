#include "days/day05.h"
#include <gtest/gtest.h>

static std::vector<std::string> RgusExampleInput() {
    return {"3-5", "10-14", "16-20", "12-18", "", "1", "5", "8", "11", "17", "32"};
}

TEST(Day05, ExamplePart1) {
    Day05 slvDay;
    slvDay.SetInput(RgusExampleInput());
    EXPECT_EQ(slvDay.TxtPart1(), "3");
}

TEST(Day05, ExamplePart2) {
    Day05 slvDay;
    slvDay.SetInput(RgusExampleInput());
    EXPECT_EQ(slvDay.TxtPart2(), "14");
}