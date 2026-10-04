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
    std::istringstream istringstreamInput("3-5\r\n\r\n  12 \r\nlast");
    EXPECT_EQ(core::ReadLines(istringstreamInput),
              (std::vector<std::string>{"3-5", "", "  12 ", "last"}));
}

TEST(Parse, RejectsPartialAndOutOfRangeIntegers) {
    EXPECT_EQ(core::ParseInteger<int>(" 42\r"), 42);
    EXPECT_THROW(core::ParseInteger<int>("2junk"), std::invalid_argument);
    EXPECT_THROW(core::ParseInteger<int>(""), std::invalid_argument);
    EXPECT_THROW(core::ParseInteger<int>("999999999999999999999"), std::invalid_argument);
}

TEST(ParallelSum, ProcessesEveryIndexExactlyOnce) {
    std::vector<std::atomic_int> vectorVisitCounts(1000);
    EXPECT_EQ(core::ParallelSumIndexed(vectorVisitCounts.size(),
                                       [&](std::size_t uItemIndex) {
                                           ++vectorVisitCounts[uItemIndex];
                                           return static_cast<std::int64_t>(uItemIndex);
                                       }),
              499500);
    for (const auto& atomicVisitCount : vectorVisitCounts)
        EXPECT_EQ(atomicVisitCount.load(), 1);
    EXPECT_EQ(core::ParallelSumIndexed(0, [](std::size_t) { return 99; }), 0);
    EXPECT_EQ(core::ParallelSumIndexed(1, [](std::size_t) { return 99; }), 99);
}

TEST(ParallelSum, PropagatesWorkerExceptions) {
    EXPECT_THROW(
        core::ParallelSumIndexed(
            100, [](std::size_t) -> std::int64_t { throw std::runtime_error("worker failed"); }),
        std::runtime_error);
}

TEST(Registry, AllDaysAreRegisteredAndHandleEmptyInput) {
    EXPECT_EQ(Registry::Instance().ImplementedDays(),
              (std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    for (int iDay : Registry::Instance().ImplementedDays()) {
        SCOPED_TRACE(iDay);
        auto psolution = Registry::Instance().Make(iDay);
        ASSERT_TRUE(psolution);
        psolution->SetInput({});
        EXPECT_EQ(psolution->Part1(), "0");
        EXPECT_EQ(psolution->Part2(), "0");
    }
}
