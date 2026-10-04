#include "days/day11.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <sstream>
#include <stdexcept>
#include <unordered_set>

// Registration
namespace {
const core::DayRegistration<Day11> registration{11};

constexpr int first_required_visit_bit = 1;
constexpr int second_required_visit_bit = 2;
constexpr int all_required_visits_mask = first_required_visit_bit | second_required_visit_bit;
} // namespace

// ------------------------------------------------------------
// Parsing
// ------------------------------------------------------------

void Day11::set_input(const std::vector<std::string>& input_lines) {
    outputs_by_device_.clear();

    for (const auto& line : input_lines) {
        if (line.empty())
            continue;

        auto colon_offset = line.find(':');
        if (colon_offset == std::string::npos)
            throw std::invalid_argument("Expected a device and its outputs");

        std::string source_device(core::trim(std::string_view(line).substr(0, colon_offset)));
        if (source_device.empty())
            throw std::invalid_argument("Missing device name");
        std::string output_names = line.substr(colon_offset + 1);

        std::istringstream outputs(output_names);
        std::string device_name;
        std::vector<std::string> output_devices;

        while (outputs >> device_name) {
            output_devices.push_back(device_name);
        }

        outputs_by_device_[source_device] = std::move(output_devices);
    }
}

// ------------------------------------------------------------
// Part 1 — count all paths from "you" to "out"
// ------------------------------------------------------------

std::int64_t
Day11::count_paths_from(const std::string& device,
                        std::unordered_map<std::string, std::int64_t>& path_counts_by_device,
                        std::unordered_set<std::string>& devices_on_path) {
    if (device == "out") {
        return 1;
    }

    if (auto found = path_counts_by_device.find(device); found != path_counts_by_device.end()) {
        return found->second;
    }

    // cycle guard (should not happen for valid input)
    if (!devices_on_path.insert(device).second) {
        throw std::invalid_argument("Device graph contains a cycle");
    }

    std::int64_t path_count = 0;
    if (const auto found = outputs_by_device_.find(device); found != outputs_by_device_.end()) {
        for (const auto& next_device : found->second)
            path_count += count_paths_from(next_device, path_counts_by_device, devices_on_path);
    }

    devices_on_path.erase(device);
    path_counts_by_device[device] = path_count;
    return path_count;
}

std::string Day11::part1() {
    if (outputs_by_device_.empty()) {
        return "0";
    }

    std::unordered_map<std::string, std::int64_t> path_counts_by_device;
    std::unordered_set<std::string> devices_on_path;

    std::int64_t path_count = count_paths_from("you", path_counts_by_device, devices_on_path);
    return std::to_string(path_count);
}

// ------------------------------------------------------------
// Part 2 — paths from "svr" to "out" that visit both "dac" and "fft"
// ------------------------------------------------------------

std::int64_t Day11::count_paths_through_required_devices(
    const std::string& start_device, const std::string& end_device,
    const std::string& first_required_device, const std::string& second_required_device) {
    if (outputs_by_device_.empty()) {
        return 0;
    }

    std::unordered_map<VisitState, std::int64_t, VisitStateHash> path_counts_by_state;
    std::unordered_set<VisitState, VisitStateHash> states_on_path;

    int initial_visit_mask = 0;
    if (start_device == first_required_device)
        initial_visit_mask |= first_required_visit_bit;
    if (start_device == second_required_device)
        initial_visit_mask |= second_required_visit_bit;

    const auto count_paths = [&](auto&& recurse, const std::string& device,
                                 int required_visit_mask) -> std::int64_t {
        VisitState state{device, required_visit_mask};

        if (auto found = path_counts_by_state.find(state); found != path_counts_by_state.end()) {
            return found->second;
        }

        if (device == end_device) {
            return path_counts_by_state[state] =
                       (required_visit_mask == all_required_visits_mask ? 1 : 0);
        }

        if (!states_on_path.insert(state).second)
            throw std::invalid_argument("Device graph contains a cycle");
        std::int64_t path_count = 0;
        if (const auto found = outputs_by_device_.find(device); found != outputs_by_device_.end()) {
            for (const auto& next_device : found->second) {
                int next_visit_mask = required_visit_mask;
                if (next_device == first_required_device)
                    next_visit_mask |= first_required_visit_bit;
                if (next_device == second_required_device)
                    next_visit_mask |= second_required_visit_bit;
                path_count += recurse(recurse, next_device, next_visit_mask);
            }
        }
        states_on_path.erase(state);
        path_counts_by_state[state] = path_count;
        return path_count;
    };

    return count_paths(count_paths, start_device, initial_visit_mask);
}

std::string Day11::part2() {
    std::int64_t path_count = count_paths_through_required_devices("svr", "out", "dac", "fft");
    return std::to_string(path_count);
}
