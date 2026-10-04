#pragma once

#include <cstdint>

#include "core/Solution.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Day11 final : public Solution {
  public:
    void set_input(const std::vector<std::string>& input_lines) override;
    std::string part1() override;
    std::string part2() override;

  private:
    // adjacency list
    std::unordered_map<std::string, std::vector<std::string>> outputs_by_device_;

    // ---------- Part 1 ----------
    std::int64_t
    count_paths_from(const std::string& device,
                     std::unordered_map<std::string, std::int64_t>& path_counts_by_device,
                     std::unordered_set<std::string>& devices_on_path);

    // ---------- Part 2 ----------
    struct VisitState {
        std::string device;
        int required_visit_mask;

        bool operator==(const VisitState& other) const {
            return device == other.device && required_visit_mask == other.required_visit_mask;
        }
    };

    struct VisitStateHash {
        std::size_t operator()(const VisitState& state) const {
            return std::hash<std::string>()(state.device) ^
                   (std::hash<int>()(state.required_visit_mask) << 1);
        }
    };

    std::int64_t count_paths_through_required_devices(const std::string& start_device,
                                                      const std::string& end_device,
                                                      const std::string& first_required_device,
                                                      const std::string& second_required_device);
};
