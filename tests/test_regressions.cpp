#include "core/Registry.h"
#include "core/Solution.h"
#include "days/day02.h"
#include "days/day04.h"
#include "days/day05.h"
#include "days/day07.h"
#include "days/day08.h"
#include "days/day09.h"
#include "days/day10.h"
#include "days/day11.h"
#include "days/day12.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <gtest/gtest.h>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

TEST(Day01, ArithmeticMatchesClickSimulation) {
    auto psolutionSolver = Registry::Instance().Make(1);
    std::mt19937 mt19937Rotations(2025);
    std::vector<std::string> vectorInputLines;
    int iDialPosition = 50, iZeroEndpoints = 0, iZeroCrossings = 0;
    for (int iRotationIndex = 0; iRotationIndex < 2000; ++iRotationIndex) {
        const bool bTurnsLeft = mt19937Rotations() % 2;
        const int iClickCount = mt19937Rotations() % 500;
        vectorInputLines.push_back(std::string(bTurnsLeft ? "L" : "R") +
                                   std::to_string(iClickCount));
        for (int iClickIndex = 0; iClickIndex < iClickCount; ++iClickIndex) {
            iDialPosition = (iDialPosition + (bTurnsLeft ? 99 : 1)) % 100;
            iZeroCrossings += iDialPosition == 0;
        }
        iZeroEndpoints += iDialPosition == 0;
    }
    psolutionSolver->SetInput(vectorInputLines);
    EXPECT_EQ(psolutionSolver->Part1(), std::to_string(iZeroEndpoints));
    EXPECT_EQ(psolutionSolver->Part2(), std::to_string(iZeroCrossings));
}

TEST(Day01, LargeRotationsAndStartingOnZero) {
    auto psolutionSolver = Registry::Instance().Make(1);
    psolutionSolver->SetInput({"L50", "L0", "L1000000000000", "R1000000000000"});
    EXPECT_EQ(psolutionSolver->Part1(), "4");
    EXPECT_EQ(psolutionSolver->Part2(), "20000000001");
    EXPECT_THROW(psolutionSolver->SetInput({"Q10"}), std::invalid_argument);
    EXPECT_THROW(psolutionSolver->SetInput({"L-1"}), std::invalid_argument);
}

TEST(Day02, HandlesEighteenAndNineteenDigitIds) {
    Day02 day02Solver;
    day02Solver.SetInput({"111111111111111111-111111111111111111"});
    EXPECT_EQ(day02Solver.Part1(), "111111111111111111");
    EXPECT_EQ(day02Solver.Part2(), "111111111111111111");
    day02Solver.SetInput({"1111111111111111111-1111111111111111111"});
    EXPECT_EQ(day02Solver.Part1(), "0");
    EXPECT_EQ(day02Solver.Part2(), "1111111111111111111");
    day02Solver.SetInput({"9223372036854775807-9223372036854775807"});
    EXPECT_EQ(day02Solver.Part1(), "0");
    EXPECT_EQ(day02Solver.Part2(), "0");
    EXPECT_THROW(day02Solver.SetInput({"11"}), std::invalid_argument);
    EXPECT_THROW(day02Solver.SetInput({"22-11"}), std::invalid_argument);
    EXPECT_THROW(day02Solver.SetInput({"1-2oops"}), std::invalid_argument);
}

TEST(Day04, RejectsRaggedGrid) {
    Day04 day04Solver;
    EXPECT_THROW(day04Solver.SetInput({"@@@", "@"}), std::invalid_argument);
}

TEST(Day05, HandlesEmptyRangesAndInputReuse) {
    Day05 day05Solver;
    day05Solver.SetInput({"1-5", "", "3"});
    EXPECT_EQ(day05Solver.Part1(), "1");
    day05Solver.SetInput({"", "3"});
    EXPECT_EQ(day05Solver.Part1(), "0");
    EXPECT_EQ(day05Solver.Part2(), "0");
    EXPECT_THROW(day05Solver.SetInput({"5-1"}), std::invalid_argument);
}

TEST(Day07, CountsTimelinesThatExitTheSides) {
    Day07 day07Solver;
    day07Solver.SetInput({"S", "^"});
    EXPECT_EQ(day07Solver.Part1(), "1");
    EXPECT_EQ(day07Solver.Part2(), "2");
    day07Solver.SetInput({});
    EXPECT_EQ(day07Solver.Part1(), "0");
    EXPECT_EQ(day07Solver.Part2(), "0");
    EXPECT_THROW(day07Solver.SetInput({"..."}), std::invalid_argument);
}

TEST(Day08, RejectsMalformedCoordinates) {
    Day08 day08Solver;
    EXPECT_THROW(day08Solver.SetInput({"1;2;3"}), std::invalid_argument);
    EXPECT_THROW(day08Solver.SetInput({"1,2"}), std::invalid_argument);
}

TEST(Day09, RectangleAreaExceedsThirtyTwoBits) {
    Day09 day09Solver;
    day09Solver.SetInput({"0,0", "100000,0", "100000,100000", "0,100000"});
    EXPECT_EQ(day09Solver.Part1(), "10000200001");
    EXPECT_EQ(day09Solver.Part2(), "10000200001");
}

TEST(Day09, CoordinateDifferenceExceedsThirtyTwoBits) {
    Day09 day09Solver;
    day09Solver.SetInput({"-2000000000,0", "2000000000,0", "2000000000,1", "-2000000000,1"});
    EXPECT_EQ(day09Solver.Part1(), "8000000002");
    EXPECT_EQ(day09Solver.Part2(), "8000000002");
}

TEST(Day10, RejectsImpossibleTargets) {
    Day10 day10Solver;
    day10Solver.SetInput({"[#.] (0,1) {1,2}"});
    EXPECT_THROW(day10Solver.Part1(), std::runtime_error);
    EXPECT_THROW(day10Solver.Part2(), std::runtime_error);
    day10Solver.SetInput({"[...] (0,1) (0,2) (1,2) {1,1,1}"});
    EXPECT_THROW(day10Solver.Part2(), std::runtime_error);
}

TEST(Day10, RejectsMalformedButtons) {
    Day10 day10Solver;
    EXPECT_THROW(day10Solver.SetInput({"[#] (-1) {1}"}), std::invalid_argument);
    EXPECT_THROW(day10Solver.SetInput({"[#] (1) {1}"}), std::invalid_argument);
    EXPECT_THROW(day10Solver.SetInput({"[#] (0 {1}"}), std::invalid_argument);
    EXPECT_THROW(day10Solver.SetInput({"[#] (0,0) {1}"}), std::invalid_argument);
}

TEST(Day10, RepeatedButtonsAndLargePressCounts) {
    Day10 day10Solver;
    std::string stringMachine = "[#]";
    for (int iButtonIndex = 0; iButtonIndex < 40; ++iButtonIndex)
        stringMachine += " (0)";
    day10Solver.SetInput({stringMachine + " {5}"});
    EXPECT_EQ(day10Solver.Part1(), "1");
    EXPECT_EQ(day10Solver.Part2(), "5");
    day10Solver.SetInput({"[##] (0) (1) {2000000000,2000000000}"});
    EXPECT_EQ(day10Solver.Part2(), "4000000000");
}

TEST(Day11, RejectsCyclesInsteadOfRecursingForever) {
    Day11 day11Solver;
    day11Solver.SetInput({"you: a", "svr: a", "a: b", "b: a out"});
    EXPECT_THROW(day11Solver.Part1(), std::invalid_argument);
    EXPECT_THROW(day11Solver.Part2(), std::invalid_argument);
}

TEST(Day11, MissingDestinationsAreDeadEnds) {
    Day11 day11Solver;
    day11Solver.SetInput({"you: missing out", "svr: fft", "fft: dac", "dac: missing out"});
    EXPECT_EQ(day11Solver.Part1(), "1");
    EXPECT_EQ(day11Solver.Part2(), "1");
    EXPECT_EQ(day11Solver.Part1(), "1");
}

TEST(Day12, LargeAreaDoesNotGuaranteeFit) {
    Day12 day12Solver;
    day12Solver.SetInput({"0:", "##", "##", "", "1x300: 1", "4x100: 100"});
    EXPECT_EQ(day12Solver.Part1(), "1");
}

TEST(Day12, ExactPackingAlsoAppliesToLargeRegions) {
    Day12 day12Solver;
    std::vector<std::string> vectorInputLines{"0:"};
    for (int iShapeRow = 0; iShapeRow < 10; ++iShapeRow)
        vectorInputLines.emplace_back(10, '#');
    vectorInputLines.push_back("15x16: 2");
    day12Solver.SetInput(vectorInputLines);
    EXPECT_EQ(day12Solver.Part1(), "0");
}

TEST(Day12, ValidatesRegionCountsAndDimensions) {
    Day12 day12Solver;
    EXPECT_THROW(day12Solver.SetInput({"0:", "#", "2x2: 1 2"}), std::invalid_argument);
    EXPECT_THROW(day12Solver.SetInput({"0:", "#", "2x2: -1"}), std::invalid_argument);
    EXPECT_THROW(day12Solver.SetInput({"0:", "#", "0x2: 1"}), std::invalid_argument);
}

TEST(Day10, ExactSolversMatchExhaustiveSearchOnSmallMachines) {
    std::mt19937 mt19937Machines(102025);
    for (int iSampleIndex = 0; iSampleIndex < 200; ++iSampleIndex) {
        SCOPED_TRACE(iSampleIndex);
        const int iCounterCount = 3;
        const int iButtonCount = 1 + mt19937Machines() % 5;
        std::vector<int> vectorButtonMasks(iButtonCount);
        for (auto& iButtonMask : vectorButtonMasks)
            iButtonMask = 1 + mt19937Machines() % 7;
        std::vector<int> vectorJoltageRequirements(iCounterCount);
        for (auto& iRequiredJoltage : vectorJoltageRequirements)
            iRequiredJoltage = mt19937Machines() % 5;
        const int iTargetLightsMask = mt19937Machines() % 8;
        std::string stringMachine = "[";
        for (int iCounterIndex = 0; iCounterIndex < iCounterCount; ++iCounterIndex)
            stringMachine += iTargetLightsMask & (1 << iCounterIndex) ? '#' : '.';
        stringMachine += ']';
        for (int iButtonMask : vectorButtonMasks) {
            stringMachine += " (";
            bool bFirstCounter = true;
            for (int iCounterIndex = 0; iCounterIndex < iCounterCount; ++iCounterIndex) {
                if (!(iButtonMask & (1 << iCounterIndex)))
                    continue;
                if (!bFirstCounter)
                    stringMachine += ',';
                bFirstCounter = false;
                stringMachine += std::to_string(iCounterIndex);
            }
            stringMachine += ')';
        }
        stringMachine += " {" + std::to_string(vectorJoltageRequirements[0]) + ',' +
                         std::to_string(vectorJoltageRequirements[1]) + ',' +
                         std::to_string(vectorJoltageRequirements[2]) + '}';

        int iExpectedLightPresses = 100;
        for (int iPressedButtonsMask = 0; iPressedButtonsMask < (1 << iButtonCount);
             ++iPressedButtonsMask) {
            int iActualLightsMask = 0, iPressCount = 0;
            for (int iButtonIndex = 0; iButtonIndex < iButtonCount; ++iButtonIndex) {
                if (iPressedButtonsMask & (1 << iButtonIndex)) {
                    iActualLightsMask ^= vectorButtonMasks[iButtonIndex];
                    ++iPressCount;
                }
            }
            if (iActualLightsMask == iTargetLightsMask)
                iExpectedLightPresses = std::min(iExpectedLightPresses, iPressCount);
        }
        int iExpectedJoltagePresses = 100;
        std::vector<int> vectorActualJoltages(iCounterCount, 0);
        const auto exhaustive_search_ = [&](auto&& recurse_, int iButtonIndex,
                                            int iPressCount) -> void {
            if (iButtonIndex == iButtonCount) {
                if (vectorActualJoltages == vectorJoltageRequirements)
                    iExpectedJoltagePresses = std::min(iExpectedJoltagePresses, iPressCount);
                return;
            }
            for (int iRepetitions = 0; iRepetitions <= 4; ++iRepetitions) {
                for (int iCounterIndex = 0; iCounterIndex < iCounterCount; ++iCounterIndex)
                    if (vectorButtonMasks[iButtonIndex] & (1 << iCounterIndex))
                        vectorActualJoltages[iCounterIndex] += iRepetitions;
                recurse_(recurse_, iButtonIndex + 1, iPressCount + iRepetitions);
                for (int iCounterIndex = 0; iCounterIndex < iCounterCount; ++iCounterIndex)
                    if (vectorButtonMasks[iButtonIndex] & (1 << iCounterIndex))
                        vectorActualJoltages[iCounterIndex] -= iRepetitions;
            }
        };
        exhaustive_search_(exhaustive_search_, 0, 0);

        Day10 day10Solver;
        day10Solver.SetInput({stringMachine});
        if (iExpectedLightPresses == 100)
            EXPECT_THROW(day10Solver.Part1(), std::runtime_error);
        else
            EXPECT_EQ(day10Solver.Part1(), std::to_string(iExpectedLightPresses));
        if (iExpectedJoltagePresses == 100)
            EXPECT_THROW(day10Solver.Part2(), std::runtime_error);
        else
            EXPECT_EQ(day10Solver.Part2(), std::to_string(iExpectedJoltagePresses));
    }
}

TEST(Day02, MatchesDirectRepeatedDigitDetection) {
    std::mt19937 mt19937Ranges(22025);
    for (int iSampleIndex = 0; iSampleIndex < 100; ++iSampleIndex) {
        const int iFirstId = mt19937Ranges() % 100000;
        const int iLastId = iFirstId + mt19937Ranges() % 500;
        std::int64_t iExpectedPart1 = 0, iExpectedPart2 = 0;
        for (int iCandidateId = iFirstId; iCandidateId <= iLastId; ++iCandidateId) {
            const auto stringDigits = std::to_string(iCandidateId);
            if (stringDigits.size() % 2 == 0 && stringDigits.substr(0, stringDigits.size() / 2) ==
                                                    stringDigits.substr(stringDigits.size() / 2))
                iExpectedPart1 += iCandidateId;
            for (std::size_t uBlockLength = 1; uBlockLength <= stringDigits.size() / 2;
                 ++uBlockLength) {
                if (stringDigits.size() % uBlockLength != 0)
                    continue;
                std::string stringRepeated;
                for (std::size_t uRepetitionIndex = 0;
                     uRepetitionIndex < stringDigits.size() / uBlockLength; ++uRepetitionIndex)
                    stringRepeated += stringDigits.substr(0, uBlockLength);
                if (stringRepeated == stringDigits) {
                    iExpectedPart2 += iCandidateId;
                    break;
                }
            }
        }
        Day02 day02Solver;
        day02Solver.SetInput({std::to_string(iFirstId) + '-' + std::to_string(iLastId)});
        EXPECT_EQ(day02Solver.Part1(), std::to_string(iExpectedPart1));
        EXPECT_EQ(day02Solver.Part2(), std::to_string(iExpectedPart2));
    }
}

TEST(Day09, ConcavePolygonsMatchExhaustiveTileChecks) {
    std::mt19937 mt19937Polygons(92025);
    for (int iSampleIndex = 0; iSampleIndex < 100; ++iSampleIndex) {
        std::vector<int> vectorStripHeights(3 + mt19937Polygons() % 4);
        for (auto& iHeight : vectorStripHeights)
            iHeight = 1 + mt19937Polygons() % 6;
        const int iWidth = static_cast<int>(vectorStripHeights.size()) * 2;
        std::vector<std::pair<int, int>> vectorVertices{
            {0, 0}, {iWidth, 0}, {iWidth, vectorStripHeights.back()}};
        for (int iStripIndex = static_cast<int>(vectorStripHeights.size()) - 1; iStripIndex > 0;
             --iStripIndex) {
            vectorVertices.emplace_back(2 * iStripIndex, vectorStripHeights[iStripIndex]);
            vectorVertices.emplace_back(2 * iStripIndex, vectorStripHeights[iStripIndex - 1]);
        }
        vectorVertices.emplace_back(0, vectorStripHeights.front());
        std::vector<std::string> vectorInputLines;
        for (const auto& [iX, iY] : vectorVertices)
            vectorInputLines.push_back(std::to_string(iX) + ',' + std::to_string(iY));
        int iExpectedArea = 0;
        for (std::size_t uFirstVertexIndex = 0; uFirstVertexIndex < vectorVertices.size();
             ++uFirstVertexIndex) {
            for (std::size_t uSecondVertexIndex = uFirstVertexIndex + 1;
                 uSecondVertexIndex < vectorVertices.size(); ++uSecondVertexIndex) {
                const auto [iFirstX, iLastX] =
                    std::minmax(vectorVertices[uFirstVertexIndex].first,
                                vectorVertices[uSecondVertexIndex].first);
                const auto [iFirstY, iLastY] =
                    std::minmax(vectorVertices[uFirstVertexIndex].second,
                                vectorVertices[uSecondVertexIndex].second);
                bool bInside = true;
                for (int iX = iFirstX; iX <= iLastX; ++iX) {
                    for (int iY = iFirstY; iY <= iLastY; ++iY) {
                        bool bAllowedTile = false;
                        for (int iStripIndex = 0;
                             iStripIndex < static_cast<int>(vectorStripHeights.size());
                             ++iStripIndex)
                            bAllowedTile |= iX >= 2 * iStripIndex && iX <= 2 * iStripIndex + 2 &&
                                            iY <= vectorStripHeights[iStripIndex];
                        bInside &= bAllowedTile;
                    }
                }
                if (bInside)
                    iExpectedArea =
                        std::max(iExpectedArea, (iLastX - iFirstX + 1) * (iLastY - iFirstY + 1));
            }
        }
        Day09 day09Solver;
        day09Solver.SetInput(vectorInputLines);
        EXPECT_EQ(day09Solver.Part2(), std::to_string(iExpectedArea)) << "sample " << iSampleIndex;
    }
}

TEST(Day08, DetectsArithmeticOverflow) {
    Day08 day08Solver;
    EXPECT_THROW(day08Solver.SetInput({"-9223372036854775808,0,0", "9223372036854775807,0,0"}),
                 std::overflow_error);
    day08Solver.SetInput({"4000000000,0,0", "4000000001,0,0"});
    EXPECT_THROW(day08Solver.Part2(), std::overflow_error);
}

TEST(Day12, PackingMatchesExhaustiveSmallBoards) {
    std::mt19937 mt19937Boards(122025);
    for (int iSampleIndex = 0; iSampleIndex < 100; ++iSampleIndex) {
        const int iColumnCount = 1 + mt19937Boards() % 4, iRowCount = 1 + mt19937Boards() % 4;
        const int iCornerCount = mt19937Boards() % 4, iDominoCount = mt19937Boards() % 4;
        std::array<std::vector<std::uint32_t>, 2> arrayPlacementMasksByShape;
        for (int iRow = 0; iRow < iRowCount; ++iRow) {
            for (int iColumn = 0; iColumn < iColumnCount; ++iColumn) {
                const auto uCellBit = std::uint32_t{1} << (iRow * iColumnCount + iColumn);
                if (iColumn + 1 < iColumnCount)
                    arrayPlacementMasksByShape[1].push_back(uCellBit | (uCellBit << 1));
                if (iRow + 1 < iRowCount)
                    arrayPlacementMasksByShape[1].push_back(uCellBit | (uCellBit << iColumnCount));
                if (iColumn + 1 < iColumnCount && iRow + 1 < iRowCount) {
                    const auto uSquareMask = uCellBit | (uCellBit << 1) |
                                             (uCellBit << iColumnCount) |
                                             (uCellBit << (iColumnCount + 1));
                    for (int iCellOffset : {0, 1, iColumnCount, iColumnCount + 1})
                        arrayPlacementMasksByShape[0].push_back(uSquareMask ^
                                                                (uCellBit << iCellOffset));
                }
            }
        }
        const auto exhaustive_search_ = [&](auto&& recurse_, std::uint32_t uOccupiedMask,
                                            int iRemainingCorners, int iRemainingDominoes) -> bool {
            if (iRemainingCorners == 0 && iRemainingDominoes == 0)
                return true;
            const int iShapeIndex = iRemainingCorners > 0 ? 0 : 1;
            for (auto uPlacementMask : arrayPlacementMasksByShape[iShapeIndex]) {
                if ((uPlacementMask & uOccupiedMask) == 0 &&
                    recurse_(recurse_, uOccupiedMask | uPlacementMask,
                             iRemainingCorners - (iShapeIndex == 0),
                             iRemainingDominoes - (iShapeIndex == 1)))
                    return true;
            }
            return false;
        };
        const bool bCanFit = 3 * iCornerCount + 2 * iDominoCount <= iColumnCount * iRowCount &&
                             exhaustive_search_(exhaustive_search_, 0, iCornerCount, iDominoCount);
        Day12 day12Solver;
        day12Solver.SetInput({"0:", "##", "#.", "", "1:", "##", "",
                              std::to_string(iColumnCount) + 'x' + std::to_string(iRowCount) +
                                  ": " + std::to_string(iCornerCount) + ' ' +
                                  std::to_string(iDominoCount)});
        EXPECT_EQ(day12Solver.Part1(), bCanFit ? "1" : "0") << "sample " << iSampleIndex;
    }
}
