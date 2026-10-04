#include "core/Input.h"
#include "core/Parallel.h"
#include "core/Parse.h"
#include "core/Registry.h"
#include "core/Solution.h"

#include <atomic>
#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
#include <vector>

TEST(Input, PreservesBlankLinesAndSpacingAndRemovesCarriageReturns) {
    std::istringstream input("3-5\r\n\r\n  12 \r\nlast");
    EXPECT_EQ(core::read_lines(input), (std::vector<std::string>{"3-5", "", "  12 ", "last"}));
}

TEST(Parse, RejectsPartialAndOutOfRangeIntegers) {
    EXPECT_EQ(core::parse_integer<int>(" 42\r"), 42);
    EXPECT_THROW(core::parse_integer<int>("2junk"), std::invalid_argument);
    EXPECT_THROW(core::parse_integer<int>(""), std::invalid_argument);
    EXPECT_THROW(core::parse_integer<int>("999999999999999999999"), std::invalid_argument);
}

TEST(ParallelSum, ProcessesEveryIndexExactlyOnce) {
    std::vector<std::atomic_int> visit_counts(1000);
    EXPECT_EQ(core::parallel_sum_indexed(visit_counts.size(),
                                         [&](std::size_t item_index) {
                                             ++visit_counts[item_index];
                                             return static_cast<std::int64_t>(item_index);
                                         }),
              499500);
    for (const auto& visit_count : visit_counts)
        EXPECT_EQ(visit_count.load(), 1);
    EXPECT_EQ(core::parallel_sum_indexed(0, [](std::size_t) { return 99; }), 0);
    EXPECT_EQ(core::parallel_sum_indexed(1, [](std::size_t) { return 99; }), 99);
}

TEST(ParallelSum, PropagatesWorkerExceptions) {
    EXPECT_THROW(
        core::parallel_sum_indexed(
            100, [](std::size_t) -> std::int64_t { throw std::runtime_error("worker failed"); }),
        std::runtime_error);
}

TEST(Registry, AllDaysAreRegisteredAndHandleEmptyInput) {
    EXPECT_EQ(Registry::instance().implemented_days(),
              (std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    for (int day : Registry::instance().implemented_days()) {
        SCOPED_TRACE(day);
        auto solver = Registry::instance().make(day);
        ASSERT_TRUE(solver);
        solver->set_input({});
        EXPECT_EQ(solver->part1(), "0");
        EXPECT_EQ(solver->part2(), "0");
    }
}
