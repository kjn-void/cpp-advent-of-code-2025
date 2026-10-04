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

    connections = sorted_connections(junction_boxes);
}

// -----------------------------------------------------------
// Distance & Edge Preparation
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

std::vector<Day08::Connection>
Day08::sorted_connections(std::span<const JunctionBox> junction_boxes) {
    const int box_count = static_cast<int>(junction_boxes.size());
    std::vector<Connection> connections;
    if (box_count > 1)
        connections.reserve(junction_boxes.size() * (junction_boxes.size() - 1) / 2);

    for (int first_box_index = 0; first_box_index < box_count; ++first_box_index) {
        for (int second_box_index = first_box_index + 1; second_box_index < box_count;
             ++second_box_index) {
            connections.push_back({squared_distance(junction_boxes[first_box_index],
                                                    junction_boxes[second_box_index]),
                                   first_box_index, second_box_index});
        }
    }

    std::ranges::sort(
        connections, [](const Connection& first_connection, const Connection& second_connection) {
            return std::tie(first_connection.squared_distance, first_connection.first_box_index,
                            first_connection.second_box_index) <
                   std::tie(second_connection.squared_distance, second_connection.first_box_index,
                            second_connection.second_box_index);
        });

    return connections;
}

// -----------------------------------------------------------
// DSU
// -----------------------------------------------------------

Day08::CircuitSet::CircuitSet(int box_count)
    : parent_by_box(box_count), box_count_by_root(box_count, 1) {
    for (int first_box_index = 0; first_box_index < box_count; ++first_box_index)
        parent_by_box[first_box_index] = first_box_index;
}

int Day08::CircuitSet::find_circuit(int circuit_root) {
    while (parent_by_box[circuit_root] != circuit_root) {
        parent_by_box[circuit_root] = parent_by_box[parent_by_box[circuit_root]];
        circuit_root = parent_by_box[circuit_root];
    }
    return circuit_root;
}

bool Day08::CircuitSet::join_circuits(int first_root, int second_root) {
    first_root = find_circuit(first_root);
    second_root = find_circuit(second_root);
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
                                                        std::span<const Connection> connections,
                                                        int connection_limit) {
    if (junction_boxes.empty())
        return {};

    CircuitSet circuits(static_cast<int>(junction_boxes.size()));
    connection_limit = std::min(connection_limit, static_cast<int>(connections.size()));

    for (int connection_index = 0; connection_index < connection_limit; ++connection_index) {
        circuits.join_circuits(connections[connection_index].first_box_index,
                               connections[connection_index].second_box_index);
    }

    std::vector<int> circuit_sizes;
    for (int first_box_index = 0; first_box_index < static_cast<int>(junction_boxes.size());
         ++first_box_index) {
        if (circuits.find_circuit(first_box_index) == first_box_index)
            circuit_sizes.push_back(circuits.box_count_by_root[first_box_index]);
    }

    std::ranges::sort(circuit_sizes, std::greater<>{});
    return circuit_sizes;
}

std::pair<int, int> Day08::connect_all_junction_boxes(std::span<const JunctionBox> junction_boxes,
                                                      std::span<const Connection> connections) {
    if (junction_boxes.size() < 2)
        return {0, 0};

    CircuitSet circuits(static_cast<int>(junction_boxes.size()));
    int circuit_count = static_cast<int>(junction_boxes.size());
    int last_first_box_index = 0, last_second_box_index = 0;

    for (const auto& connection : connections) {
        if (circuits.join_circuits(connection.first_box_index, connection.second_box_index)) {
            --circuit_count;
            last_first_box_index = connection.first_box_index;
            last_second_box_index = connection.second_box_index;
            if (circuit_count == 1)
                break;
        }
    }

    return {last_first_box_index, last_second_box_index};
}

// -----------------------------------------------------------
// Parts
// -----------------------------------------------------------

std::string Day08::part1() {
    auto circuit_sizes = circuit_sizes_after_connections(junction_boxes, connections, 1000);
    if (circuit_sizes.size() < 3)
        return "0";

    std::int64_t largest_circuits_product = std::int64_t(circuit_sizes[0]) *
                                            std::int64_t(circuit_sizes[1]) *
                                            std::int64_t(circuit_sizes[2]);

    return std::to_string(largest_circuits_product);
}

std::string Day08::part2() {
    if (junction_boxes.size() < 2)
        return "0";

    auto [first_box_index, second_box_index] =
        connect_all_junction_boxes(junction_boxes, connections);
    const auto first_x = junction_boxes[first_box_index].x;
    const auto second_x = junction_boxes[second_box_index].x;
    const auto magnitude = [](std::int64_t signed_value) {
        const auto bits = static_cast<std::uint64_t>(signed_value);
        return signed_value < 0 ? std::uint64_t{0} - bits : bits;
    };
    const bool negative = (first_x < 0) != (second_x < 0);
    const auto max_magnitude =
        static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + negative;
    const auto first_magnitude = magnitude(first_x), second_magnitude = magnitude(second_x);
    if (second_magnitude != 0 && first_magnitude > max_magnitude / second_magnitude)
        throw std::overflow_error("Junction coordinate product exceeds int64_t");
    const auto product_magnitude = first_magnitude * second_magnitude;
    if (negative && product_magnitude == max_magnitude)
        return std::to_string(std::numeric_limits<std::int64_t>::min());
    const auto signed_product = static_cast<std::int64_t>(product_magnitude);
    return std::to_string(negative ? -signed_product : signed_product);
}
