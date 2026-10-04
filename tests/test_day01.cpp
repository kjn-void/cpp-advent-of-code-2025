#include <gtest/gtest.h>

#include "core/Registry.h"
#include "core/Solution.h"

TEST(Day01, ExamplePart1) {
    auto psolution = Registry::Instance().Make(1);
    ASSERT_TRUE(psolution);

    psolution->SetInput({
        "L68",
        "L30",
        "R48",
    });

    EXPECT_EQ(psolution->Part1(), "1");
}

TEST(Day01, ExamplePart2) {
    auto psolution = Registry::Instance().Make(1);
    ASSERT_TRUE(psolution);

    psolution->SetInput({
        "L68",
        "L30",
        "R48",
        "L5",
        "R60",
        "L55",
        "L1",
        "L99",
        "R14",
        "L82",
    });

    EXPECT_EQ(psolution->Part2(), "6");
}