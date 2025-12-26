#include "days/day11.h"
#include "core/Registry.h"

#include <sstream>

// 🔌 Registration
static bool registered = [] {
    Registry::instance().register_day(
        11, [] { return std::make_unique<Day11>(); }
    );
    return true;
}();

// ------------------------------------------------------------
// Parsing
// ------------------------------------------------------------

void Day11::set_input(const std::vector<std::string>& lines) {
    adj.clear();

    for (auto line : lines) {
        if (line.empty()) continue;

        auto pos = line.find(':');
        if (pos == std::string::npos) continue;

        std::string from = line.substr(0, pos);
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

int64_t Day11::count_paths_from(
    const std::string& node,
    std::unordered_map<std::string, int64_t>& memo,
    std::unordered_map<std::string, bool>& visiting
) {
    if (node == "out") {
        return 1;
    }

    if (auto it = memo.find(node); it != memo.end()) {
        return it->second;
    }

    // cycle guard (should not happen for valid input)
    if (visiting[node]) {
        return 0;
    }

    visiting[node] = true;

    int64_t total = 0;
    for (const auto& next : adj[node]) {
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

    std::unordered_map<std::string, int64_t> memo;
    std::unordered_map<std::string, bool> visiting;

    int64_t total = count_paths_from("you", memo, visiting);
    return std::to_string(total);
}

// ------------------------------------------------------------
// Part 2 — paths that visit both required nodes
// ------------------------------------------------------------

int64_t Day11::count_paths_with_required(
    const std::string& start,
    const std::string& end,
    const std::string& need1,
    const std::string& need2
) {
    if (adj.empty()) {
        return 0;
    }

    std::unordered_map<State, int64_t, StateHash> memo;

    int initMask = 0;
    if (start == need1) initMask |= 1;
    if (start == need2) initMask |= 2;

    std::function<int64_t(const std::string&, int)> dfs;
    dfs = [&](const std::string& node, int mask) -> int64_t {
        State st{node, mask};

        if (auto it = memo.find(st); it != memo.end()) {
            return it->second;
        }

        if (node == end) {
            return memo[st] = (mask == 3 ? 1 : 0);
        }

        int64_t total = 0;
        for (const auto& nxt : adj[node]) {
            int nextMask = mask;
            if (nxt == need1) nextMask |= 1;
            if (nxt == need2) nextMask |= 2;
            total += dfs(nxt, nextMask);
        }

        memo[st] = total;
        return total;
    };

    return dfs(start, initMask);
}

std::string Day11::part2() {
    int64_t total = count_paths_with_required("svr", "out", "dac", "fft");
    return std::to_string(total);
}