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
    std::mt19937 rotations_generator(2025);
    std::vector<std::string> input_lines;
    int dial_position = 50, zero_endpoints = 0, zero_crossings = 0;
    for (int rotation_index = 0; rotation_index < 2000; ++rotation_index) {
        const bool turns_left = rotations_generator() % 2;
        const int click_count = rotations_generator() % 500;
        input_lines.push_back(std::string(turns_left ? "L" : "R") + std::to_string(click_count));
        for (int click_index = 0; click_index < click_count; ++click_index) {
            dial_position = (dial_position + (turns_left ? 99 : 1)) % 100;
            zero_crossings += dial_position == 0;
        }
        zero_endpoints += dial_position == 0;
    }
    solver->set_input(input_lines);
    EXPECT_EQ(solver->part1(), std::to_string(zero_endpoints));
    EXPECT_EQ(solver->part2(), std::to_string(zero_crossings));
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
    Day02 solver;
    solver.set_input({"111111111111111111-111111111111111111"});
    EXPECT_EQ(solver.part1(), "111111111111111111");
    EXPECT_EQ(solver.part2(), "111111111111111111");
    solver.set_input({"1111111111111111111-1111111111111111111"});
    EXPECT_EQ(solver.part1(), "0");
    EXPECT_EQ(solver.part2(), "1111111111111111111");
    solver.set_input({"9223372036854775807-9223372036854775807"});
    EXPECT_EQ(solver.part1(), "0");
    EXPECT_EQ(solver.part2(), "0");
    EXPECT_THROW(solver.set_input({"11"}), std::invalid_argument);
    EXPECT_THROW(solver.set_input({"22-11"}), std::invalid_argument);
    EXPECT_THROW(solver.set_input({"1-2oops"}), std::invalid_argument);
}

TEST(Day04, RejectsRaggedGrid) {
    Day04 solver;
    EXPECT_THROW(solver.set_input({"@@@", "@"}), std::invalid_argument);
}

TEST(Day05, HandlesEmptyRangesAndInputReuse) {
    Day05 solver;
    solver.set_input({"1-5", "", "3"});
    EXPECT_EQ(solver.part1(), "1");
    solver.set_input({"", "3"});
    EXPECT_EQ(solver.part1(), "0");
    EXPECT_EQ(solver.part2(), "0");
    EXPECT_THROW(solver.set_input({"5-1"}), std::invalid_argument);
}

TEST(Day07, CountsTimelinesThatExitTheSides) {
    Day07 solver;
    solver.set_input({"S", "^"});
    EXPECT_EQ(solver.part1(), "1");
    EXPECT_EQ(solver.part2(), "2");
    solver.set_input({});
    EXPECT_EQ(solver.part1(), "0");
    EXPECT_EQ(solver.part2(), "0");
    EXPECT_THROW(solver.set_input({"..."}), std::invalid_argument);
}

TEST(Day08, RejectsMalformedCoordinates) {
    Day08 solver;
    EXPECT_THROW(solver.set_input({"1;2;3"}), std::invalid_argument);
    EXPECT_THROW(solver.set_input({"1,2"}), std::invalid_argument);
}

TEST(Day09, RectangleAreaExceedsThirtyTwoBits) {
    Day09 solver;
    solver.set_input({"0,0", "100000,0", "100000,100000", "0,100000"});
    EXPECT_EQ(solver.part1(), "10000200001");
    EXPECT_EQ(solver.part2(), "10000200001");
}

TEST(Day09, CoordinateDifferenceExceedsThirtyTwoBits) {
    Day09 solver;
    solver.set_input({"-2000000000,0", "2000000000,0", "2000000000,1", "-2000000000,1"});
    EXPECT_EQ(solver.part1(), "8000000002");
    EXPECT_EQ(solver.part2(), "8000000002");
}

TEST(Day10, RejectsImpossibleTargets) {
    Day10 solver;
    solver.set_input({"[#.] (0,1) {1,2}"});
    EXPECT_THROW(solver.part1(), std::runtime_error);
    EXPECT_THROW(solver.part2(), std::runtime_error);
    solver.set_input({"[...] (0,1) (0,2) (1,2) {1,1,1}"});
    EXPECT_THROW(solver.part2(), std::runtime_error);
}

TEST(Day10, RejectsMalformedButtons) {
    Day10 solver;
    EXPECT_THROW(solver.set_input({"[#] (-1) {1}"}), std::invalid_argument);
    EXPECT_THROW(solver.set_input({"[#] (1) {1}"}), std::invalid_argument);
    EXPECT_THROW(solver.set_input({"[#] (0 {1}"}), std::invalid_argument);
    EXPECT_THROW(solver.set_input({"[#] (0,0) {1}"}), std::invalid_argument);
}

TEST(Day10, RepeatedButtonsAndLargePressCounts) {
    Day10 solver;
    std::string machine_text = "[#]";
    for (int button_index = 0; button_index < 40; ++button_index)
        machine_text += " (0)";
    solver.set_input({machine_text + " {5}"});
    EXPECT_EQ(solver.part1(), "1");
    EXPECT_EQ(solver.part2(), "5");
    solver.set_input({"[##] (0) (1) {2000000000,2000000000}"});
    EXPECT_EQ(solver.part2(), "4000000000");
}

TEST(Day11, RejectsCyclesInsteadOfRecursingForever) {
    Day11 solver;
    solver.set_input({"you: a", "svr: a", "a: b", "b: a out"});
    EXPECT_THROW(solver.part1(), std::invalid_argument);
    EXPECT_THROW(solver.part2(), std::invalid_argument);
}

TEST(Day11, MissingDestinationsAreDeadEnds) {
    Day11 solver;
    solver.set_input({"you: missing out", "svr: fft", "fft: dac", "dac: missing out"});
    EXPECT_EQ(solver.part1(), "1");
    EXPECT_EQ(solver.part2(), "1");
    EXPECT_EQ(solver.part1(), "1");
}

TEST(Day12, LargeAreaDoesNotGuaranteeFit) {
    Day12 solver;
    solver.set_input({"0:", "##", "##", "", "1x300: 1", "4x100: 100"});
    EXPECT_EQ(solver.part1(), "1");
}

TEST(Day12, ExactPackingAlsoAppliesToLargeRegions) {
    Day12 solver;
    std::vector<std::string> input_lines{"0:"};
    for (int shape_row = 0; shape_row < 10; ++shape_row)
        input_lines.emplace_back(10, '#');
    input_lines.push_back("15x16: 2");
    solver.set_input(input_lines);
    EXPECT_EQ(solver.part1(), "0");
}

TEST(Day12, ValidatesRegionCountsAndDimensions) {
    Day12 solver;
    EXPECT_THROW(solver.set_input({"0:", "#", "2x2: 1 2"}), std::invalid_argument);
    EXPECT_THROW(solver.set_input({"0:", "#", "2x2: -1"}), std::invalid_argument);
    EXPECT_THROW(solver.set_input({"0:", "#", "0x2: 1"}), std::invalid_argument);
}

TEST(Day10, ExactSolversMatchExhaustiveSearchOnSmallMachines) {
    std::mt19937 machines_generator(102025);
    for (int sample_index = 0; sample_index < 200; ++sample_index) {
        SCOPED_TRACE(sample_index);
        const int counter_count = 3;
        const int button_count = 1 + machines_generator() % 5;
        std::vector<int> button_masks(button_count);
        for (auto& button_mask : button_masks)
            button_mask = 1 + machines_generator() % 7;
        std::vector<int> joltage_requirements(counter_count);
        for (auto& required_joltage : joltage_requirements)
            required_joltage = machines_generator() % 5;
        const int target_lights_mask = machines_generator() % 8;
        std::string machine_text = "[";
        for (int counter_index = 0; counter_index < counter_count; ++counter_index)
            machine_text += target_lights_mask & (1 << counter_index) ? '#' : '.';
        machine_text += ']';
        for (int button_mask : button_masks) {
            machine_text += " (";
            bool first_counter = true;
            for (int counter_index = 0; counter_index < counter_count; ++counter_index) {
                if (!(button_mask & (1 << counter_index)))
                    continue;
                if (!first_counter)
                    machine_text += ',';
                first_counter = false;
                machine_text += std::to_string(counter_index);
            }
            machine_text += ')';
        }
        machine_text += " {" + std::to_string(joltage_requirements[0]) + ',' +
                        std::to_string(joltage_requirements[1]) + ',' +
                        std::to_string(joltage_requirements[2]) + '}';

        int expected_light_presses = 100;
        for (int pressed_buttons_mask = 0; pressed_buttons_mask < (1 << button_count);
             ++pressed_buttons_mask) {
            int actual_lights_mask = 0, press_count = 0;
            for (int button_index = 0; button_index < button_count; ++button_index) {
                if (pressed_buttons_mask & (1 << button_index)) {
                    actual_lights_mask ^= button_masks[button_index];
                    ++press_count;
                }
            }
            if (actual_lights_mask == target_lights_mask)
                expected_light_presses = std::min(expected_light_presses, press_count);
        }
        int expected_joltage_presses = 100;
        std::vector<int> actual_joltages(counter_count, 0);
        const auto exhaustive_search = [&](auto&& recurse, int button_index,
                                           int press_count) -> void {
            if (button_index == button_count) {
                if (actual_joltages == joltage_requirements)
                    expected_joltage_presses = std::min(expected_joltage_presses, press_count);
                return;
            }
            for (int repetitions = 0; repetitions <= 4; ++repetitions) {
                for (int counter_index = 0; counter_index < counter_count; ++counter_index)
                    if (button_masks[button_index] & (1 << counter_index))
                        actual_joltages[counter_index] += repetitions;
                recurse(recurse, button_index + 1, press_count + repetitions);
                for (int counter_index = 0; counter_index < counter_count; ++counter_index)
                    if (button_masks[button_index] & (1 << counter_index))
                        actual_joltages[counter_index] -= repetitions;
            }
        };
        exhaustive_search(exhaustive_search, 0, 0);

        Day10 solver;
        solver.set_input({machine_text});
        if (expected_light_presses == 100)
            EXPECT_THROW(solver.part1(), std::runtime_error);
        else
            EXPECT_EQ(solver.part1(), std::to_string(expected_light_presses));
        if (expected_joltage_presses == 100)
            EXPECT_THROW(solver.part2(), std::runtime_error);
        else
            EXPECT_EQ(solver.part2(), std::to_string(expected_joltage_presses));
    }
}

TEST(Day02, MatchesDirectRepeatedDigitDetection) {
    std::mt19937 ranges_generator(22025);
    for (int sample_index = 0; sample_index < 100; ++sample_index) {
        const int first_id = ranges_generator() % 100000;
        const int last_id = first_id + ranges_generator() % 500;
        std::int64_t expected_part1 = 0, expected_part2 = 0;
        for (int candidate_id = first_id; candidate_id <= last_id; ++candidate_id) {
            const auto digits_text = std::to_string(candidate_id);
            if (digits_text.size() % 2 == 0 && digits_text.substr(0, digits_text.size() / 2) ==
                                                   digits_text.substr(digits_text.size() / 2))
                expected_part1 += candidate_id;
            for (std::size_t block_length = 1; block_length <= digits_text.size() / 2;
                 ++block_length) {
                if (digits_text.size() % block_length != 0)
                    continue;
                std::string repeated_text;
                for (std::size_t repetition_index = 0;
                     repetition_index < digits_text.size() / block_length; ++repetition_index)
                    repeated_text += digits_text.substr(0, block_length);
                if (repeated_text == digits_text) {
                    expected_part2 += candidate_id;
                    break;
                }
            }
        }
        Day02 solver;
        solver.set_input({std::to_string(first_id) + '-' + std::to_string(last_id)});
        EXPECT_EQ(solver.part1(), std::to_string(expected_part1));
        EXPECT_EQ(solver.part2(), std::to_string(expected_part2));
    }
}

TEST(Day09, ConcavePolygonsMatchExhaustiveTileChecks) {
    std::mt19937 polygons_generator(92025);
    for (int sample_index = 0; sample_index < 100; ++sample_index) {
        std::vector<int> strip_heights(3 + polygons_generator() % 4);
        for (auto& height : strip_heights)
            height = 1 + polygons_generator() % 6;
        const int width = static_cast<int>(strip_heights.size()) * 2;
        std::vector<std::pair<int, int>> vertices{
            {0, 0}, {width, 0}, {width, strip_heights.back()}};
        for (int strip_index = static_cast<int>(strip_heights.size()) - 1; strip_index > 0;
             --strip_index) {
            vertices.emplace_back(2 * strip_index, strip_heights[strip_index]);
            vertices.emplace_back(2 * strip_index, strip_heights[strip_index - 1]);
        }
        vertices.emplace_back(0, strip_heights.front());
        std::vector<std::string> input_lines;
        for (const auto& [x, y] : vertices)
            input_lines.push_back(std::to_string(x) + ',' + std::to_string(y));
        int expected_area = 0;
        for (std::size_t first_vertex_index = 0; first_vertex_index < vertices.size();
             ++first_vertex_index) {
            for (std::size_t second_vertex_index = first_vertex_index + 1;
                 second_vertex_index < vertices.size(); ++second_vertex_index) {
                const auto [first_x, last_x] = std::minmax(vertices[first_vertex_index].first,
                                                           vertices[second_vertex_index].first);
                const auto [first_y, last_y] = std::minmax(vertices[first_vertex_index].second,
                                                           vertices[second_vertex_index].second);
                bool inside = true;
                for (int x = first_x; x <= last_x; ++x) {
                    for (int y = first_y; y <= last_y; ++y) {
                        bool allowed_tile = false;
                        for (int strip_index = 0;
                             strip_index < static_cast<int>(strip_heights.size()); ++strip_index)
                            allowed_tile |= x >= 2 * strip_index && x <= 2 * strip_index + 2 &&
                                            y <= strip_heights[strip_index];
                        inside &= allowed_tile;
                    }
                }
                if (inside)
                    expected_area =
                        std::max(expected_area, (last_x - first_x + 1) * (last_y - first_y + 1));
            }
        }
        Day09 solver;
        solver.set_input(input_lines);
        EXPECT_EQ(solver.part2(), std::to_string(expected_area)) << "sample " << sample_index;
    }
}

TEST(Day08, DetectsArithmeticOverflow) {
    Day08 solver;
    EXPECT_THROW(solver.set_input({"-9223372036854775808,0,0", "9223372036854775807,0,0"}),
                 std::overflow_error);
    solver.set_input({"4000000000,0,0", "4000000001,0,0"});
    EXPECT_THROW(solver.part2(), std::overflow_error);
}

TEST(Day12, PackingMatchesExhaustiveSmallBoards) {
    std::mt19937 boards_generator(122025);
    for (int sample_index = 0; sample_index < 100; ++sample_index) {
        const int column_count = 1 + boards_generator() % 4, row_count = 1 + boards_generator() % 4;
        const int corner_count = boards_generator() % 4, domino_count = boards_generator() % 4;
        std::array<std::vector<std::uint32_t>, 2> placement_masks_by_shape;
        for (int row = 0; row < row_count; ++row) {
            for (int column = 0; column < column_count; ++column) {
                const auto cell_bit = std::uint32_t{1} << (row * column_count + column);
                if (column + 1 < column_count)
                    placement_masks_by_shape[1].push_back(cell_bit | (cell_bit << 1));
                if (row + 1 < row_count)
                    placement_masks_by_shape[1].push_back(cell_bit | (cell_bit << column_count));
                if (column + 1 < column_count && row + 1 < row_count) {
                    const auto square_mask = cell_bit | (cell_bit << 1) |
                                             (cell_bit << column_count) |
                                             (cell_bit << (column_count + 1));
                    for (int cell_offset : {0, 1, column_count, column_count + 1})
                        placement_masks_by_shape[0].push_back(square_mask ^
                                                              (cell_bit << cell_offset));
                }
            }
        }
        const auto exhaustive_search = [&](auto&& recurse, std::uint32_t occupied_mask,
                                           int remaining_corners, int remaining_dominoes) -> bool {
            if (remaining_corners == 0 && remaining_dominoes == 0)
                return true;
            const int shape_index = remaining_corners > 0 ? 0 : 1;
            for (auto placement_mask : placement_masks_by_shape[shape_index]) {
                if ((placement_mask & occupied_mask) == 0 &&
                    recurse(recurse, occupied_mask | placement_mask,
                            remaining_corners - (shape_index == 0),
                            remaining_dominoes - (shape_index == 1)))
                    return true;
            }
            return false;
        };
        const bool can_fit = 3 * corner_count + 2 * domino_count <= column_count * row_count &&
                             exhaustive_search(exhaustive_search, 0, corner_count, domino_count);
        Day12 solver;
        solver.set_input({"0:", "##", "#.", "", "1:", "##", "",
                          std::to_string(column_count) + 'x' + std::to_string(row_count) + ": " +
                              std::to_string(corner_count) + ' ' + std::to_string(domino_count)});
        EXPECT_EQ(solver.part1(), can_fit ? "1" : "0") << "sample " << sample_index;
    }
}
