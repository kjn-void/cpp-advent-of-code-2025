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
    auto solver = Registry::instance().make(1);
    std::mt19937 random(2025);
    std::vector<std::string> lines;
    int position = 50, endpoints = 0, crossings = 0;
    for (int i = 0; i < 2000; ++i) {
        const bool left = random() % 2;
        const int distance = random() % 500;
        lines.push_back(std::string(left ? "L" : "R") + std::to_string(distance));
        for (int step = 0; step < distance; ++step) {
            position = (position + (left ? 99 : 1)) % 100;
            crossings += position == 0;
        }
        endpoints += position == 0;
    }
    solver->set_input(lines);
    EXPECT_EQ(solver->part1(), std::to_string(endpoints));
    EXPECT_EQ(solver->part2(), std::to_string(crossings));
}

TEST(Day01, LargeRotationsAndStartingOnZero) {
    auto solver = Registry::instance().make(1);
    solver->set_input({"L50", "L0", "L1000000000000", "R1000000000000"});
    EXPECT_EQ(solver->part1(), "4");
    EXPECT_EQ(solver->part2(), "20000000001");
    EXPECT_THROW(solver->set_input({"Q10"}), std::invalid_argument);
    EXPECT_THROW(solver->set_input({"L-1"}), std::invalid_argument);
}

TEST(Day02, HandlesEighteenAndNineteenDigitIds) {
    Day02 day;
    day.set_input({"111111111111111111-111111111111111111"});
    EXPECT_EQ(day.part1(), "111111111111111111");
    EXPECT_EQ(day.part2(), "111111111111111111");
    day.set_input({"1111111111111111111-1111111111111111111"});
    EXPECT_EQ(day.part1(), "0");
    EXPECT_EQ(day.part2(), "1111111111111111111");
    day.set_input({"9223372036854775807-9223372036854775807"});
    EXPECT_EQ(day.part1(), "0");
    EXPECT_EQ(day.part2(), "0");
    EXPECT_THROW(day.set_input({"11"}), std::invalid_argument);
    EXPECT_THROW(day.set_input({"22-11"}), std::invalid_argument);
    EXPECT_THROW(day.set_input({"1-2oops"}), std::invalid_argument);
}

TEST(Day04, RejectsRaggedGrid) {
    Day04 day;
    EXPECT_THROW(day.set_input({"@@@", "@"}), std::invalid_argument);
}

TEST(Day05, HandlesEmptyRangesAndInputReuse) {
    Day05 day;
    day.set_input({"1-5", "", "3"});
    EXPECT_EQ(day.part1(), "1");
    day.set_input({"", "3"});
    EXPECT_EQ(day.part1(), "0");
    EXPECT_EQ(day.part2(), "0");
    EXPECT_THROW(day.set_input({"5-1"}), std::invalid_argument);
}

TEST(Day07, CountsTimelinesThatExitTheSides) {
    Day07 day;
    day.set_input({"S", "^"});
    EXPECT_EQ(day.part1(), "1");
    EXPECT_EQ(day.part2(), "2");
    day.set_input({});
    EXPECT_EQ(day.part1(), "0");
    EXPECT_EQ(day.part2(), "0");
    EXPECT_THROW(day.set_input({"..."}), std::invalid_argument);
}

TEST(Day08, RejectsMalformedCoordinates) {
    Day08 day;
    EXPECT_THROW(day.set_input({"1;2;3"}), std::invalid_argument);
    EXPECT_THROW(day.set_input({"1,2"}), std::invalid_argument);
}

TEST(Day09, RectangleAreaExceedsThirtyTwoBits) {
    Day09 day;
    day.set_input({"0,0", "100000,0", "100000,100000", "0,100000"});
    EXPECT_EQ(day.part1(), "10000200001");
    EXPECT_EQ(day.part2(), "10000200001");
}

TEST(Day09, CoordinateDifferenceExceedsThirtyTwoBits) {
    Day09 day;
    day.set_input({"-2000000000,0", "2000000000,0", "2000000000,1", "-2000000000,1"});
    EXPECT_EQ(day.part1(), "8000000002");
    EXPECT_EQ(day.part2(), "8000000002");
}

TEST(Day10, RejectsImpossibleTargets) {
    Day10 day;
    day.set_input({"[#.] (0,1) {1,2}"});
    EXPECT_THROW(day.part1(), std::runtime_error);
    EXPECT_THROW(day.part2(), std::runtime_error);
    day.set_input({"[...] (0,1) (0,2) (1,2) {1,1,1}"});
    EXPECT_THROW(day.part2(), std::runtime_error);
}

TEST(Day10, RejectsMalformedButtons) {
    Day10 day;
    EXPECT_THROW(day.set_input({"[#] (-1) {1}"}), std::invalid_argument);
    EXPECT_THROW(day.set_input({"[#] (1) {1}"}), std::invalid_argument);
    EXPECT_THROW(day.set_input({"[#] (0 {1}"}), std::invalid_argument);
    EXPECT_THROW(day.set_input({"[#] (0,0) {1}"}), std::invalid_argument);
}

TEST(Day10, RepeatedButtonsAndLargePressCounts) {
    Day10 day;
    std::string line = "[#]";
    for (int i = 0; i < 40; ++i)
        line += " (0)";
    day.set_input({line + " {5}"});
    EXPECT_EQ(day.part1(), "1");
    EXPECT_EQ(day.part2(), "5");
    day.set_input({"[##] (0) (1) {2000000000,2000000000}"});
    EXPECT_EQ(day.part2(), "4000000000");
}

TEST(Day11, RejectsCyclesInsteadOfRecursingForever) {
    Day11 day;
    day.set_input({"you: a", "svr: a", "a: b", "b: a out"});
    EXPECT_THROW(day.part1(), std::invalid_argument);
    EXPECT_THROW(day.part2(), std::invalid_argument);
}

TEST(Day11, MissingDestinationsAreDeadEnds) {
    Day11 day;
    day.set_input({"you: missing out", "svr: fft", "fft: dac", "dac: missing out"});
    EXPECT_EQ(day.part1(), "1");
    EXPECT_EQ(day.part2(), "1");
    EXPECT_EQ(day.part1(), "1");
}

TEST(Day12, LargeAreaDoesNotGuaranteeFit) {
    Day12 day;
    day.set_input({"0:", "##", "##", "", "1x300: 1", "4x100: 100"});
    EXPECT_EQ(day.part1(), "1");
}

TEST(Day12, ExactPackingAlsoAppliesToLargeRegions) {
    Day12 day;
    std::vector<std::string> input{"0:"};
    for (int i = 0; i < 10; ++i)
        input.emplace_back(10, '#');
    input.push_back("15x16: 2");
    day.set_input(input);
    EXPECT_EQ(day.part1(), "0");
}

TEST(Day12, ValidatesRegionCountsAndDimensions) {
    Day12 day;
    EXPECT_THROW(day.set_input({"0:", "#", "2x2: 1 2"}), std::invalid_argument);
    EXPECT_THROW(day.set_input({"0:", "#", "2x2: -1"}), std::invalid_argument);
    EXPECT_THROW(day.set_input({"0:", "#", "0x2: 1"}), std::invalid_argument);
}

TEST(Day10, ExactSolversMatchExhaustiveSearchOnSmallMachines) {
    std::mt19937 random(102025);
    for (int sample = 0; sample < 200; ++sample) {
        SCOPED_TRACE(sample);
        const int counters = 3;
        const int button_count = 1 + random() % 5;
        std::vector<int> buttons(button_count);
        for (auto& button : buttons)
            button = 1 + random() % 7;
        std::vector<int> target(counters);
        for (auto& counter : target)
            counter = random() % 5;
        const int lights = random() % 8;
        std::string line = "[";
        for (int i = 0; i < counters; ++i)
            line += lights & (1 << i) ? '#' : '.';
        line += ']';
        for (int button : buttons) {
            line += " (";
            bool first = true;
            for (int i = 0; i < counters; ++i) {
                if (!(button & (1 << i)))
                    continue;
                if (!first)
                    line += ',';
                first = false;
                line += std::to_string(i);
            }
            line += ')';
        }
        line += " {" + std::to_string(target[0]) + ',' + std::to_string(target[1]) + ',' +
                std::to_string(target[2]) + '}';

        int best_lights = 100;
        for (int mask = 0; mask < (1 << button_count); ++mask) {
            int actual = 0, presses = 0;
            for (int j = 0; j < button_count; ++j) {
                if (mask & (1 << j)) {
                    actual ^= buttons[j];
                    ++presses;
                }
            }
            if (actual == lights)
                best_lights = std::min(best_lights, presses);
        }
        int best_joltage = 100;
        std::vector<int> actual(counters, 0);
        const auto exhaustive = [&](auto&& self, int button, int presses) -> void {
            if (button == button_count) {
                if (actual == target)
                    best_joltage = std::min(best_joltage, presses);
                return;
            }
            for (int count = 0; count <= 4; ++count) {
                for (int i = 0; i < counters; ++i)
                    if (buttons[button] & (1 << i))
                        actual[i] += count;
                self(self, button + 1, presses + count);
                for (int i = 0; i < counters; ++i)
                    if (buttons[button] & (1 << i))
                        actual[i] -= count;
            }
        };
        exhaustive(exhaustive, 0, 0);

        Day10 day;
        day.set_input({line});
        if (best_lights == 100)
            EXPECT_THROW(day.part1(), std::runtime_error);
        else
            EXPECT_EQ(day.part1(), std::to_string(best_lights));
        if (best_joltage == 100)
            EXPECT_THROW(day.part2(), std::runtime_error);
        else
            EXPECT_EQ(day.part2(), std::to_string(best_joltage));
    }
}

TEST(Day02, MatchesDirectRepeatedDigitDetection) {
    std::mt19937 random(22025);
    for (int sample = 0; sample < 100; ++sample) {
        const int low = random() % 100000;
        const int high = low + random() % 500;
        std::int64_t first = 0, second = 0;
        for (int value = low; value <= high; ++value) {
            const auto digits = std::to_string(value);
            if (digits.size() % 2 == 0 &&
                digits.substr(0, digits.size() / 2) == digits.substr(digits.size() / 2))
                first += value;
            for (std::size_t length = 1; length <= digits.size() / 2; ++length) {
                if (digits.size() % length != 0)
                    continue;
                std::string repeated;
                for (std::size_t i = 0; i < digits.size() / length; ++i)
                    repeated += digits.substr(0, length);
                if (repeated == digits) {
                    second += value;
                    break;
                }
            }
        }
        Day02 day;
        day.set_input({std::to_string(low) + '-' + std::to_string(high)});
        EXPECT_EQ(day.part1(), std::to_string(first));
        EXPECT_EQ(day.part2(), std::to_string(second));
    }
}

TEST(Day09, ConcavePolygonsMatchExhaustiveTileChecks) {
    std::mt19937 random(92025);
    for (int sample = 0; sample < 100; ++sample) {
        std::vector<int> heights(3 + random() % 4);
        for (auto& height : heights)
            height = 1 + random() % 6;
        const int width = static_cast<int>(heights.size()) * 2;
        std::vector<std::pair<int, int>> vertices{{0, 0}, {width, 0}, {width, heights.back()}};
        for (int i = static_cast<int>(heights.size()) - 1; i > 0; --i) {
            vertices.emplace_back(2 * i, heights[i]);
            vertices.emplace_back(2 * i, heights[i - 1]);
        }
        vertices.emplace_back(0, heights.front());
        std::vector<std::string> input;
        for (const auto& [x, y] : vertices)
            input.push_back(std::to_string(x) + ',' + std::to_string(y));
        int best = 0;
        for (std::size_t i = 0; i < vertices.size(); ++i) {
            for (std::size_t j = i + 1; j < vertices.size(); ++j) {
                const auto [left, right] = std::minmax(vertices[i].first, vertices[j].first);
                const auto [bottom, top] = std::minmax(vertices[i].second, vertices[j].second);
                bool inside = true;
                for (int x = left; x <= right; ++x) {
                    for (int y = bottom; y <= top; ++y) {
                        bool tile = false;
                        for (int strip = 0; strip < static_cast<int>(heights.size()); ++strip)
                            tile |= x >= 2 * strip && x <= 2 * strip + 2 && y <= heights[strip];
                        inside &= tile;
                    }
                }
                if (inside)
                    best = std::max(best, (right - left + 1) * (top - bottom + 1));
            }
        }
        Day09 day;
        day.set_input(input);
        EXPECT_EQ(day.part2(), std::to_string(best)) << "sample " << sample;
    }
}

TEST(Day08, DetectsArithmeticOverflow) {
    Day08 day;
    EXPECT_THROW(day.set_input({"-9223372036854775808,0,0", "9223372036854775807,0,0"}),
                 std::overflow_error);
    day.set_input({"4000000000,0,0", "4000000001,0,0"});
    EXPECT_THROW(day.part2(), std::overflow_error);
}

TEST(Day12, PackingMatchesExhaustiveSmallBoards) {
    std::mt19937 random(122025);
    for (int sample = 0; sample < 100; ++sample) {
        const int width = 1 + random() % 4, height = 1 + random() % 4;
        const int corners = random() % 4, dominoes = random() % 4;
        std::array<std::vector<std::uint32_t>, 2> placements;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const auto cell = std::uint32_t{1} << (y * width + x);
                if (x + 1 < width)
                    placements[1].push_back(cell | (cell << 1));
                if (y + 1 < height)
                    placements[1].push_back(cell | (cell << width));
                if (x + 1 < width && y + 1 < height) {
                    const auto square =
                        cell | (cell << 1) | (cell << width) | (cell << (width + 1));
                    for (int offset : {0, 1, width, width + 1})
                        placements[0].push_back(square ^ (cell << offset));
                }
            }
        }
        const auto exhaustive = [&](auto&& self, std::uint32_t occupied, int left_corners,
                                    int left_dominoes) -> bool {
            if (left_corners == 0 && left_dominoes == 0)
                return true;
            const int shape = left_corners > 0 ? 0 : 1;
            for (auto placement : placements[shape]) {
                if ((placement & occupied) == 0 &&
                    self(self, occupied | placement, left_corners - (shape == 0),
                         left_dominoes - (shape == 1)))
                    return true;
            }
            return false;
        };
        const bool fits = 3 * corners + 2 * dominoes <= width * height &&
                          exhaustive(exhaustive, 0, corners, dominoes);
        Day12 day;
        day.set_input({"0:", "##", "#.", "", "1:", "##", "",
                       std::to_string(width) + 'x' + std::to_string(height) + ": " +
                           std::to_string(corners) + ' ' + std::to_string(dominoes)});
        EXPECT_EQ(day.part1(), fits ? "1" : "0") << "sample " << sample;
    }
}
