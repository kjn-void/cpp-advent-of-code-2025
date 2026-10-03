#include "days/day11.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <sstream>
#include <stdexcept>
#include <unordered_set>

// Registration
namespace {
const core::DayRegistration<Day11> registration{11};
} // namespace

// ------------------------------------------------------------
// Parsing
// ------------------------------------------------------------

void Day11::set_input(const std::vector<std::string>& lines) {
    adj.clear();

    for (const auto& line : lines) {
        if (line.empty())
            continue;

        auto pos = line.find(':');
        if (pos == std::string::npos)
            throw std::invalid_argument("Expected a device and its outputs");

        std::string from(core::trim(std::string_view(line).substr(0, pos)));
        if (from.empty())
            throw std::invalid_argument("Missing device name");
        std::string rest = line.substr(pos + 1);

        std::istringstream iss(rest);
        std::string tok;
        std::vector<std::string> outs;

        while (iss >> tok) {
            outs.push_back(tok);
        }

        adj[from] = std::move(outs);
    }
}

// ------------------------------------------------------------
// Part 1 — count all paths from "you" to "out"
// ------------------------------------------------------------

std::int64_t Day11::count_paths_from(const std::string& node,
                                     std::unordered_map<std::string, std::int64_t>& memo,
                                     std::unordered_set<std::string>& visiting) {
    if (node == "out") {
        return 1;
    }

    if (auto it = memo.find(node); it != memo.end()) {
        return it->second;
    }

    // cycle guard (should not happen for valid input)
    if (!visiting.insert(node).second) {
        throw std::invalid_argument("Device graph contains a cycle");
    }

    std::int64_t total = 0;
    if (const auto it = adj.find(node); it != adj.end()) {
        for (const auto& next : it->second)
            total += count_paths_from(next, memo, visiting);
    }

    visiting.erase(node);
    memo[node] = total;
    return total;
}

std::string Day11::part1() {
    if (adj.empty()) {
        return "0";
    }

    std::unordered_map<std::string, std::int64_t> memo;
    std::unordered_set<std::string> visiting;

    std::int64_t total = count_paths_from("you", memo, visiting);
    return std::to_string(total);
}

// ------------------------------------------------------------
// Part 2 — paths that visit both required nodes
// ------------------------------------------------------------

std::int64_t Day11::count_paths_with_required(const std::string& start, const std::string& end,
                                              const std::string& need1, const std::string& need2) {
    if (adj.empty()) {
        return 0;
    }

    std::unordered_map<State, std::int64_t, StateHash> memo;
    std::unordered_set<State, StateHash> visiting;

    int initMask = 0;
    if (start == need1)
        initMask |= 1;
    if (start == need2)
        initMask |= 2;

    const auto dfs = [&](auto&& self, const std::string& node, int mask) -> std::int64_t {
        State st{node, mask};

        if (auto it = memo.find(st); it != memo.end()) {
            return it->second;
        }

        if (node == end) {
            return memo[st] = (mask == 3 ? 1 : 0);
        }

        if (!visiting.insert(st).second)
            throw std::invalid_argument("Device graph contains a cycle");
        std::int64_t total = 0;
        if (const auto it = adj.find(node); it != adj.end()) {
            for (const auto& nxt : it->second) {
                int nextMask = mask;
                if (nxt == need1)
                    nextMask |= 1;
                if (nxt == need2)
                    nextMask |= 2;
                total += self(self, nxt, nextMask);
            }
        }
        visiting.erase(st);
        memo[st] = total;
        return total;
    };

    return dfs(dfs, start, initMask);
}

std::string Day11::part2() {
    std::int64_t total = count_paths_with_required("svr", "out", "dac", "fft");
    return std::to_string(total);
}
