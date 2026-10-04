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
    auto pslvDay = Regy::RegyInstance().PslvMake(1);
    std::mt19937 genRotations(2025);
    std::vector<std::string> rgusLines;
    int posDial = 50, cntEndpoints = 0, cntCrossings = 0;
    for (int iterRotation = 0; iterRotation < 2000; ++iterRotation) {
        const bool fLeft = genRotations() % 2;
        const int cclkRotation = genRotations() % 500;
        rgusLines.push_back(std::string(fLeft ? "L" : "R") + std::to_string(cclkRotation));
        for (int iterClick = 0; iterClick < cclkRotation; ++iterClick) {
            posDial = (posDial + (fLeft ? 99 : 1)) % 100;
            cntCrossings += posDial == 0;
        }
        cntEndpoints += posDial == 0;
    }
    pslvDay->SetInput(rgusLines);
    EXPECT_EQ(pslvDay->TxtPart1(), std::to_string(cntEndpoints));
    EXPECT_EQ(pslvDay->TxtPart2(), std::to_string(cntCrossings));
}

TEST(Day01, LargeRotationsAndStartingOnZero) {
    auto pslvDay = Regy::RegyInstance().PslvMake(1);
    pslvDay->SetInput({"L50", "L0", "L1000000000000", "R1000000000000"});
    EXPECT_EQ(pslvDay->TxtPart1(), "4");
    EXPECT_EQ(pslvDay->TxtPart2(), "20000000001");
    EXPECT_THROW(pslvDay->SetInput({"Q10"}), std::invalid_argument);
    EXPECT_THROW(pslvDay->SetInput({"L-1"}), std::invalid_argument);
}

TEST(Day02, HandlesEighteenAndNineteenDigitIds) {
    Day02 slvDay;
    slvDay.SetInput({"111111111111111111-111111111111111111"});
    EXPECT_EQ(slvDay.TxtPart1(), "111111111111111111");
    EXPECT_EQ(slvDay.TxtPart2(), "111111111111111111");
    slvDay.SetInput({"1111111111111111111-1111111111111111111"});
    EXPECT_EQ(slvDay.TxtPart1(), "0");
    EXPECT_EQ(slvDay.TxtPart2(), "1111111111111111111");
    slvDay.SetInput({"9223372036854775807-9223372036854775807"});
    EXPECT_EQ(slvDay.TxtPart1(), "0");
    EXPECT_EQ(slvDay.TxtPart2(), "0");
    EXPECT_THROW(slvDay.SetInput({"11"}), std::invalid_argument);
    EXPECT_THROW(slvDay.SetInput({"22-11"}), std::invalid_argument);
    EXPECT_THROW(slvDay.SetInput({"1-2oops"}), std::invalid_argument);
}

TEST(Day04, RejectsRaggedGrid) {
    Day04 slvDay;
    EXPECT_THROW(slvDay.SetInput({"@@@", "@"}), std::invalid_argument);
}

TEST(Day05, HandlesEmptyRangesAndInputReuse) {
    Day05 slvDay;
    slvDay.SetInput({"1-5", "", "3"});
    EXPECT_EQ(slvDay.TxtPart1(), "1");
    slvDay.SetInput({"", "3"});
    EXPECT_EQ(slvDay.TxtPart1(), "0");
    EXPECT_EQ(slvDay.TxtPart2(), "0");
    EXPECT_THROW(slvDay.SetInput({"5-1"}), std::invalid_argument);
}

TEST(Day07, CountsTimelinesThatExitTheSides) {
    Day07 slvDay;
    slvDay.SetInput({"S", "^"});
    EXPECT_EQ(slvDay.TxtPart1(), "1");
    EXPECT_EQ(slvDay.TxtPart2(), "2");
    slvDay.SetInput({});
    EXPECT_EQ(slvDay.TxtPart1(), "0");
    EXPECT_EQ(slvDay.TxtPart2(), "0");
    EXPECT_THROW(slvDay.SetInput({"..."}), std::invalid_argument);
}

TEST(Day08, RejectsMalformedCoordinates) {
    Day08 slvDay;
    EXPECT_THROW(slvDay.SetInput({"1;2;3"}), std::invalid_argument);
    EXPECT_THROW(slvDay.SetInput({"1,2"}), std::invalid_argument);
}

TEST(Day09, RectangleAreaExceedsThirtyTwoBits) {
    Day09 slvDay;
    slvDay.SetInput({"0,0", "100000,0", "100000,100000", "0,100000"});
    EXPECT_EQ(slvDay.TxtPart1(), "10000200001");
    EXPECT_EQ(slvDay.TxtPart2(), "10000200001");
}

TEST(Day09, CoordinateDifferenceExceedsThirtyTwoBits) {
    Day09 slvDay;
    slvDay.SetInput({"-2000000000,0", "2000000000,0", "2000000000,1", "-2000000000,1"});
    EXPECT_EQ(slvDay.TxtPart1(), "8000000002");
    EXPECT_EQ(slvDay.TxtPart2(), "8000000002");
}

TEST(Day10, RejectsImpossibleTargets) {
    Day10 slvDay;
    slvDay.SetInput({"[#.] (0,1) {1,2}"});
    EXPECT_THROW(slvDay.TxtPart1(), std::runtime_error);
    EXPECT_THROW(slvDay.TxtPart2(), std::runtime_error);
    slvDay.SetInput({"[...] (0,1) (0,2) (1,2) {1,1,1}"});
    EXPECT_THROW(slvDay.TxtPart2(), std::runtime_error);
}

TEST(Day10, RejectsMalformedButtons) {
    Day10 slvDay;
    EXPECT_THROW(slvDay.SetInput({"[#] (-1) {1}"}), std::invalid_argument);
    EXPECT_THROW(slvDay.SetInput({"[#] (1) {1}"}), std::invalid_argument);
    EXPECT_THROW(slvDay.SetInput({"[#] (0 {1}"}), std::invalid_argument);
    EXPECT_THROW(slvDay.SetInput({"[#] (0,0) {1}"}), std::invalid_argument);
}

TEST(Day10, RepeatedButtonsAndLargePressCounts) {
    Day10 slvDay;
    std::string usMachine = "[#]";
    for (int ibtn = 0; ibtn < 40; ++ibtn)
        usMachine += " (0)";
    slvDay.SetInput({usMachine + " {5}"});
    EXPECT_EQ(slvDay.TxtPart1(), "1");
    EXPECT_EQ(slvDay.TxtPart2(), "5");
    slvDay.SetInput({"[##] (0) (1) {2000000000,2000000000}"});
    EXPECT_EQ(slvDay.TxtPart2(), "4000000000");
}

TEST(Day11, RejectsCyclesInsteadOfRecursingForever) {
    Day11 slvDay;
    slvDay.SetInput({"you: a", "svr: a", "a: b", "b: a out"});
    EXPECT_THROW(slvDay.TxtPart1(), std::invalid_argument);
    EXPECT_THROW(slvDay.TxtPart2(), std::invalid_argument);
}

TEST(Day11, MissingDestinationsAreDeadEnds) {
    Day11 slvDay;
    slvDay.SetInput({"you: missing out", "svr: fft", "fft: dac", "dac: missing out"});
    EXPECT_EQ(slvDay.TxtPart1(), "1");
    EXPECT_EQ(slvDay.TxtPart2(), "1");
    EXPECT_EQ(slvDay.TxtPart1(), "1");
}

TEST(Day12, LargeAreaDoesNotGuaranteeFit) {
    Day12 slvDay;
    slvDay.SetInput({"0:", "##", "##", "", "1x300: 1", "4x100: 100"});
    EXPECT_EQ(slvDay.TxtPart1(), "1");
}

TEST(Day12, ExactPackingAlsoAppliesToLargeRegions) {
    Day12 slvDay;
    std::vector<std::string> rgusLines{"0:"};
    for (int rwShape = 0; rwShape < 10; ++rwShape)
        rgusLines.emplace_back(10, '#');
    rgusLines.push_back("15x16: 2");
    slvDay.SetInput(rgusLines);
    EXPECT_EQ(slvDay.TxtPart1(), "0");
}

TEST(Day12, ValidatesRegionCountsAndDimensions) {
    Day12 slvDay;
    EXPECT_THROW(slvDay.SetInput({"0:", "#", "2x2: 1 2"}), std::invalid_argument);
    EXPECT_THROW(slvDay.SetInput({"0:", "#", "2x2: -1"}), std::invalid_argument);
    EXPECT_THROW(slvDay.SetInput({"0:", "#", "0x2: 1"}), std::invalid_argument);
}

TEST(Day10, ExactSolversMatchExhaustiveSearchOnSmallMachines) {
    std::mt19937 genMachines(102025);
    for (int iterSample = 0; iterSample < 200; ++iterSample) {
        SCOPED_TRACE(iterSample);
        const int cictr = 3;
        const int cbtn = 1 + genMachines() % 5;
        std::vector<int> rgmaskButtons(cbtn);
        for (auto& maskButton : rgmaskButtons)
            maskButton = 1 + genMachines() % 7;
        std::vector<int> jvRequired(cictr);
        for (auto& jolTarget : jvRequired)
            jolTarget = genMachines() % 5;
        const int maskTargetLights = genMachines() % 8;
        std::string usMachine = "[";
        for (int ictr = 0; ictr < cictr; ++ictr)
            usMachine += maskTargetLights & (1 << ictr) ? '#' : '.';
        usMachine += ']';
        for (int maskButton : rgmaskButtons) {
            usMachine += " (";
            bool fFirstCounter = true;
            for (int ictr = 0; ictr < cictr; ++ictr) {
                if (!(maskButton & (1 << ictr)))
                    continue;
                if (!fFirstCounter)
                    usMachine += ',';
                fFirstCounter = false;
                usMachine += std::to_string(ictr);
            }
            usMachine += ')';
        }
        usMachine += " {" + std::to_string(jvRequired[0]) + ',' + std::to_string(jvRequired[1]) +
                     ',' + std::to_string(jvRequired[2]) + '}';

        int cprLightsExpected = 100;
        for (int maskPressedButtons = 0; maskPressedButtons < (1 << cbtn); ++maskPressedButtons) {
            int maskActualLights = 0, cpr = 0;
            for (int ibtn = 0; ibtn < cbtn; ++ibtn) {
                if (maskPressedButtons & (1 << ibtn)) {
                    maskActualLights ^= rgmaskButtons[ibtn];
                    ++cpr;
                }
            }
            if (maskActualLights == maskTargetLights)
                cprLightsExpected = std::min(cprLightsExpected, cpr);
        }
        int cprJoltageExpected = 100;
        std::vector<int> jvActual(cictr, 0);
        const auto fnExhaustiveSearch = [&](auto&& fnRecurSearch, int ibtn, int cpr) -> void {
            if (ibtn == cbtn) {
                if (jvActual == jvRequired)
                    cprJoltageExpected = std::min(cprJoltageExpected, cpr);
                return;
            }
            for (int cntRepeats = 0; cntRepeats <= 4; ++cntRepeats) {
                for (int ictr = 0; ictr < cictr; ++ictr)
                    if (rgmaskButtons[ibtn] & (1 << ictr))
                        jvActual[ictr] += cntRepeats;
                fnRecurSearch(fnRecurSearch, ibtn + 1, cpr + cntRepeats);
                for (int ictr = 0; ictr < cictr; ++ictr)
                    if (rgmaskButtons[ibtn] & (1 << ictr))
                        jvActual[ictr] -= cntRepeats;
            }
        };
        fnExhaustiveSearch(fnExhaustiveSearch, 0, 0);

        Day10 slvDay;
        slvDay.SetInput({usMachine});
        if (cprLightsExpected == 100)
            EXPECT_THROW(slvDay.TxtPart1(), std::runtime_error);
        else
            EXPECT_EQ(slvDay.TxtPart1(), std::to_string(cprLightsExpected));
        if (cprJoltageExpected == 100)
            EXPECT_THROW(slvDay.TxtPart2(), std::runtime_error);
        else
            EXPECT_EQ(slvDay.TxtPart2(), std::to_string(cprJoltageExpected));
    }
}

TEST(Day02, MatchesDirectRepeatedDigitDetection) {
    std::mt19937 genRanges(22025);
    for (int iterSample = 0; iterSample < 100; ++iterSample) {
        const int idFirst = genRanges() % 100000;
        const int idLast = idFirst + genRanges() % 500;
        std::int64_t valPart1Expected = 0, valPart2Expected = 0;
        for (int idCandidate = idFirst; idCandidate <= idLast; ++idCandidate) {
            const auto txtDigits = std::to_string(idCandidate);
            if (txtDigits.size() % 2 == 0 &&
                txtDigits.substr(0, txtDigits.size() / 2) == txtDigits.substr(txtDigits.size() / 2))
                valPart1Expected += idCandidate;
            for (std::size_t lenBlock = 1; lenBlock <= txtDigits.size() / 2; ++lenBlock) {
                if (txtDigits.size() % lenBlock != 0)
                    continue;
                std::string txtRepeated;
                for (std::size_t iterRepeat = 0; iterRepeat < txtDigits.size() / lenBlock;
                     ++iterRepeat)
                    txtRepeated += txtDigits.substr(0, lenBlock);
                if (txtRepeated == txtDigits) {
                    valPart2Expected += idCandidate;
                    break;
                }
            }
        }
        Day02 slvDay;
        slvDay.SetInput({std::to_string(idFirst) + '-' + std::to_string(idLast)});
        EXPECT_EQ(slvDay.TxtPart1(), std::to_string(valPart1Expected));
        EXPECT_EQ(slvDay.TxtPart2(), std::to_string(valPart2Expected));
    }
}

TEST(Day09, ConcavePolygonsMatchExhaustiveTileChecks) {
    std::mt19937 genPolygons(92025);
    for (int iterSample = 0; iterSample < 100; ++iterSample) {
        std::vector<int> rglenHeights(3 + genPolygons() % 4);
        for (auto& lenHeight : rglenHeights)
            lenHeight = 1 + genPolygons() % 6;
        const int lenWidth = static_cast<int>(rglenHeights.size()) * 2;
        std::vector<std::pair<int, int>> rgtlVertices{
            {0, 0}, {lenWidth, 0}, {lenWidth, rglenHeights.back()}};
        for (int ilenStrip = static_cast<int>(rglenHeights.size()) - 1; ilenStrip > 0;
             --ilenStrip) {
            rgtlVertices.emplace_back(2 * ilenStrip, rglenHeights[ilenStrip]);
            rgtlVertices.emplace_back(2 * ilenStrip, rglenHeights[ilenStrip - 1]);
        }
        rgtlVertices.emplace_back(0, rglenHeights.front());
        std::vector<std::string> rgusLines;
        for (const auto& [xTile, yTile] : rgtlVertices)
            rgusLines.push_back(std::to_string(xTile) + ',' + std::to_string(yTile));
        int areaExpected = 0;
        for (std::size_t itlFirst = 0; itlFirst < rgtlVertices.size(); ++itlFirst) {
            for (std::size_t itlSecond = itlFirst + 1; itlSecond < rgtlVertices.size();
                 ++itlSecond) {
                const auto [xFirst, xLast] =
                    std::minmax(rgtlVertices[itlFirst].first, rgtlVertices[itlSecond].first);
                const auto [yFirst, yLast] =
                    std::minmax(rgtlVertices[itlFirst].second, rgtlVertices[itlSecond].second);
                bool fInside = true;
                for (int xTile = xFirst; xTile <= xLast; ++xTile) {
                    for (int yTile = yFirst; yTile <= yLast; ++yTile) {
                        bool fAllowedTile = false;
                        for (int ilenStrip = 0; ilenStrip < static_cast<int>(rglenHeights.size());
                             ++ilenStrip)
                            fAllowedTile |= xTile >= 2 * ilenStrip && xTile <= 2 * ilenStrip + 2 &&
                                            yTile <= rglenHeights[ilenStrip];
                        fInside &= fAllowedTile;
                    }
                }
                if (fInside)
                    areaExpected =
                        std::max(areaExpected, (xLast - xFirst + 1) * (yLast - yFirst + 1));
            }
        }
        Day09 slvDay;
        slvDay.SetInput(rgusLines);
        EXPECT_EQ(slvDay.TxtPart2(), std::to_string(areaExpected)) << "sample " << iterSample;
    }
}

TEST(Day08, DetectsArithmeticOverflow) {
    Day08 slvDay;
    EXPECT_THROW(slvDay.SetInput({"-9223372036854775808,0,0", "9223372036854775807,0,0"}),
                 std::overflow_error);
    slvDay.SetInput({"4000000000,0,0", "4000000001,0,0"});
    EXPECT_THROW(slvDay.TxtPart2(), std::overflow_error);
}

TEST(Day12, PackingMatchesExhaustiveSmallBoards) {
    std::mt19937 genBoards(122025);
    for (int iterSample = 0; iterSample < 100; ++iterSample) {
        const int ccol = 1 + genBoards() % 4, crw = 1 + genBoards() % 4;
        const int cntCorners = genBoards() % 4, cntDominoes = genBoards() % 4;
        std::array<std::vector<std::uint32_t>, 2> mpishprgmask;
        for (int rw = 0; rw < crw; ++rw) {
            for (int col = 0; col < ccol; ++col) {
                const auto bitCell = std::uint32_t{1} << (rw * ccol + col);
                if (col + 1 < ccol)
                    mpishprgmask[1].push_back(bitCell | (bitCell << 1));
                if (rw + 1 < crw)
                    mpishprgmask[1].push_back(bitCell | (bitCell << ccol));
                if (col + 1 < ccol && rw + 1 < crw) {
                    const auto maskSquare =
                        bitCell | (bitCell << 1) | (bitCell << ccol) | (bitCell << (ccol + 1));
                    for (int offCell : {0, 1, ccol, ccol + 1})
                        mpishprgmask[0].push_back(maskSquare ^ (bitCell << offCell));
                }
            }
        }
        const auto fnExhaustiveSearch = [&](auto&& fnRecurSearch, std::uint32_t maskOccupied,
                                            int cntRemainingCorners,
                                            int cntRemainingDominoes) -> bool {
            if (cntRemainingCorners == 0 && cntRemainingDominoes == 0)
                return true;
            const int ishp = cntRemainingCorners > 0 ? 0 : 1;
            for (auto maskPlacement : mpishprgmask[ishp]) {
                if ((maskPlacement & maskOccupied) == 0 &&
                    fnRecurSearch(fnRecurSearch, maskOccupied | maskPlacement,
                                  cntRemainingCorners - (ishp == 0),
                                  cntRemainingDominoes - (ishp == 1)))
                    return true;
            }
            return false;
        };
        const bool fCanFit = 3 * cntCorners + 2 * cntDominoes <= ccol * crw &&
                             fnExhaustiveSearch(fnExhaustiveSearch, 0, cntCorners, cntDominoes);
        Day12 slvDay;
        slvDay.SetInput({"0:", "##", "#.", "", "1:", "##", "",
                         std::to_string(ccol) + 'x' + std::to_string(crw) + ": " +
                             std::to_string(cntCorners) + ' ' + std::to_string(cntDominoes)});
        EXPECT_EQ(slvDay.TxtPart1(), fCanFit ? "1" : "0") << "sample " << iterSample;
    }
}
