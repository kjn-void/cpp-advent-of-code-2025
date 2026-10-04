#include "days/day08.h"
#include "core/Register.h"

#include <algorithm>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <tuple>

// Registration
namespace {
const core::DayRegistration<Day08> registration{8};
} // namespace

// -----------------------------------------------------------
// Parsing
// -----------------------------------------------------------

static Day08::JunctionBox parse_junction_box(const std::string& line) {
    std::stringstream coordinates(line);
    Day08::JunctionBox box{};
    char first_comma{}, second_comma{};
    if (!(coordinates >> box.x >> first_comma >> box.y >> second_comma >> box.z) ||
        first_comma != ',' || second_comma != ',' || !(coordinates >> std::ws).eof())
        throw std::invalid_argument("Expected three comma-separated coordinates");
    return box;
}

void Day08::set_input(const std::vector<std::string>& input_lines) {
    junction_boxes.clear();

    for (const auto& line : input_lines) {
        if (!line.empty()) {
            junction_boxes.push_back(parse_junction_box(line));
        }
    }

    pairs_by_distance = sort_pairs_by_distance(junction_boxes);
}

// -----------------------------------------------------------
// Distances and candidate pairs
// -----------------------------------------------------------

std::int64_t Day08::squared_distance(const JunctionBox& first_box, const JunctionBox& second_box) {
    std::int64_t squared_distance = 0;
    const auto add_squared_axis_distance = [&](std::int64_t first_coordinate,
                                               std::int64_t second_coordinate) {
        // Unsigned subtraction also handles differences spanning the signed range.
        const auto axis_distance =
            first_coordinate >= second_coordinate
                ? std::uint64_t(first_coordinate) - std::uint64_t(second_coordinate)
                : std::uint64_t(second_coordinate) - std::uint64_t(first_coordinate);
        if (axis_distance > 3037000499ULL)
            throw std::overflow_error("Squared distance exceeds int64_t");
        const auto squared_axis_distance = static_cast<std::int64_t>(axis_distance * axis_distance);
        if (squared_axis_distance > std::numeric_limits<std::int64_t>::max() - squared_distance)
            throw std::overflow_error("Squared distance exceeds int64_t");
        squared_distance += squared_axis_distance;
    };
    add_squared_axis_distance(first_box.x, second_box.x);
    add_squared_axis_distance(first_box.y, second_box.y);
    add_squared_axis_distance(first_box.z, second_box.z);
    return squared_distance;
}

std::vector<Day08::BoxPair>
Day08::sort_pairs_by_distance(std::span<const JunctionBox> junction_boxes) {
    const int box_count = static_cast<int>(junction_boxes.size());
    std::vector<BoxPair> pairs_by_distance;
    if (box_count > 1)
        pairs_by_distance.reserve(junction_boxes.size() * (junction_boxes.size() - 1) / 2);

    for (int first_box_index = 0; first_box_index < box_count; ++first_box_index) {
        for (int second_box_index = first_box_index + 1; second_box_index < box_count;
             ++second_box_index) {
            pairs_by_distance.push_back({squared_distance(junction_boxes[first_box_index],
                                                          junction_boxes[second_box_index]),
                                         first_box_index, second_box_index});
        }
    }

    std::ranges::sort(pairs_by_distance, [](const BoxPair& first_pair, const BoxPair& second_pair) {
        return std::tie(first_pair.squared_distance, first_pair.first_box_index,
                        first_pair.second_box_index) < std::tie(second_pair.squared_distance,
                                                                second_pair.first_box_index,
                                                                second_pair.second_box_index);
    });

    return pairs_by_distance;
}

// -----------------------------------------------------------
// Union-find
// -----------------------------------------------------------

Day08::CircuitSet::CircuitSet(int box_count)
    : parent_by_box(box_count), box_count_by_root(box_count, 1) {
    for (int box_index = 0; box_index < box_count; ++box_index)
        parent_by_box[box_index] = box_index;
}

int Day08::CircuitSet::find_circuit_root(int box_index) {
    while (parent_by_box[box_index] != box_index) {
        parent_by_box[box_index] = parent_by_box[parent_by_box[box_index]];
        box_index = parent_by_box[box_index];
    }
    return box_index;
}

bool Day08::CircuitSet::join_circuits(int first_box_index, int second_box_index) {
    int first_root = find_circuit_root(first_box_index);
    int second_root = find_circuit_root(second_box_index);
    if (first_root == second_root)
        return false;

    if (box_count_by_root[first_root] < box_count_by_root[second_root])
        std::swap(first_root, second_root);
    parent_by_box[second_root] = first_root;
    box_count_by_root[first_root] += box_count_by_root[second_root];
    return true;
}

// -----------------------------------------------------------
// Core helpers
// -----------------------------------------------------------

std::vector<int> Day08::circuit_sizes_after_connections(std::span<const JunctionBox> junction_boxes,
                                                        std::span<const BoxPair> pairs_by_distance,
                                                        int connection_count) {
    if (junction_boxes.empty())
        return {};

    CircuitSet circuits(static_cast<int>(junction_boxes.size()));
    connection_count = std::min(connection_count, static_cast<int>(pairs_by_distance.size()));

    for (int pair_index = 0; pair_index < connection_count; ++pair_index) {
        circuits.join_circuits(pairs_by_distance[pair_index].first_box_index,
                               pairs_by_distance[pair_index].second_box_index);
    }

    std::vector<int> circuit_sizes;
    for (int box_index = 0; box_index < static_cast<int>(junction_boxes.size()); ++box_index) {
        if (circuits.find_circuit_root(box_index) == box_index)
            circuit_sizes.push_back(circuits.box_count_by_root[box_index]);
    }

    std::ranges::sort(circuit_sizes, std::greater<>{});
    return circuit_sizes;
}

Day08::BoxPair Day08::connect_all_junction_boxes(std::span<const JunctionBox> junction_boxes,
                                                 std::span<const BoxPair> pairs_by_distance) {
    if (junction_boxes.size() < 2)
        return {0, 0, 0};

    CircuitSet circuits(static_cast<int>(junction_boxes.size()));
    int circuit_count = static_cast<int>(junction_boxes.size());
    BoxPair final_connection{0, 0, 0};

    for (const auto& box_pair : pairs_by_distance) {
        if (circuits.join_circuits(box_pair.first_box_index, box_pair.second_box_index)) {
            --circuit_count;
            final_connection = box_pair;
            if (circuit_count == 1)
                break;
        }
    }

    return final_connection;
}

// -----------------------------------------------------------
// Parts
// -----------------------------------------------------------

std::string Day08::part1() {
    auto circuit_sizes = circuit_sizes_after_connections(junction_boxes, pairs_by_distance, 1000);
    if (circuit_sizes.size() < 3)
        return "0";

    std::int64_t three_largest_circuits_product = std::int64_t(circuit_sizes[0]) *
                                                  std::int64_t(circuit_sizes[1]) *
                                                  std::int64_t(circuit_sizes[2]);

    return std::to_string(three_largest_circuits_product);
}

std::string Day08::part2() {
    if (junction_boxes.size() < 2)
        return "0";

    const auto final_connection = connect_all_junction_boxes(junction_boxes, pairs_by_distance);
    const auto first_x = junction_boxes[final_connection.first_box_index].x;
    const auto second_x = junction_boxes[final_connection.second_box_index].x;
    const auto magnitude = [](std::int64_t signed_value) {
        const auto bits = static_cast<std::uint64_t>(signed_value);
        return signed_value < 0 ? std::uint64_t{0} - bits : bits;
    };
    const bool product_is_negative = (first_x < 0) != (second_x < 0);
    const auto max_magnitude =
        static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + product_is_negative;
    const auto first_magnitude = magnitude(first_x), second_magnitude = magnitude(second_x);
    if (second_magnitude != 0 && first_magnitude > max_magnitude / second_magnitude)
        throw std::overflow_error("X coordinate product exceeds int64_t");
    const auto product_magnitude = first_magnitude * second_magnitude;
    if (product_is_negative && product_magnitude == max_magnitude)
        return std::to_string(std::numeric_limits<std::int64_t>::min());
    const auto signed_product = static_cast<std::int64_t>(product_magnitude);
    return std::to_string(product_is_negative ? -signed_product : signed_product);
}
