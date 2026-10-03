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
    std::istringstream inLines("3-5\r\n\r\n  12 \r\nlast");
    EXPECT_EQ(core::RgusReadLines(inLines), (std::vector<std::string>{"3-5", "", "  12 ", "last"}));
}

TEST(Parse, RejectsPartialAndOutOfRangeIntegers) {
    EXPECT_EQ(core::ValParseInteger<int>(" 42\r"), 42);
    EXPECT_THROW(core::ValParseInteger<int>("2junk"), std::invalid_argument);
    EXPECT_THROW(core::ValParseInteger<int>(""), std::invalid_argument);
    EXPECT_THROW(core::ValParseInteger<int>("999999999999999999999"), std::invalid_argument);
}

TEST(ParallelSum, ProcessesEveryIndexExactlyOnce) {
    std::vector<std::atomic_int> mpiitemcntVisits(1000);
    EXPECT_EQ(core::ValSumIndexed(mpiitemcntVisits.size(),
                                  [&](std::size_t iitem) {
                                      ++mpiitemcntVisits[iitem];
                                      return static_cast<std::int64_t>(iitem);
                                  }),
              499500);
    for (const auto& cntVisits : mpiitemcntVisits)
        EXPECT_EQ(cntVisits.load(), 1);
    EXPECT_EQ(core::ValSumIndexed(0, [](std::size_t) { return 99; }), 0);
    EXPECT_EQ(core::ValSumIndexed(1, [](std::size_t) { return 99; }), 99);
}

TEST(ParallelSum, PropagatesWorkerExceptions) {
    EXPECT_THROW(
        core::ValSumIndexed(
            100, [](std::size_t) -> std::int64_t { throw std::runtime_error("worker failed"); }),
        std::runtime_error);
}

TEST(Regy, AllDaysAreRegisteredAndHandleEmptyInput) {
    EXPECT_EQ(Regy::RegyInstance().RgidImplementedDays(),
              (std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
    for (int idDay : Regy::RegyInstance().RgidImplementedDays()) {
        SCOPED_TRACE(idDay);
        auto pslvDay = Regy::RegyInstance().PslvMake(idDay);
        ASSERT_TRUE(pslvDay);
        pslvDay->SetInput({});
        EXPECT_EQ(pslvDay->TxtPart1(), "0");
        EXPECT_EQ(pslvDay->TxtPart2(), "0");
    }
}
