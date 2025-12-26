#include <gtest/gtest.h>

#include "core/Registry.h"
#include "core/Solution.h"

TEST(Day01, ExamplePart1) {
    auto solver = Registry::instance().make(1);
    ASSERT_TRUE(solver);

    solver->set_input({
        "L68",
        "L30",
        "R48",
    });

    EXPECT_EQ(solver->part1(), "1");
}

TEST(Day01, ExamplePart2) {
    auto solver = Registry::instance().make(1);
    ASSERT_TRUE(solver);

    solver->set_input({
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

    EXPECT_EQ(solver->part2(), "6");
}