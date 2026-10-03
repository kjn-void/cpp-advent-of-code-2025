#include "days/day10.h"
#include "core/Parallel.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

// Registration
namespace {
const core::DayRegistration<Day10> registration{10};
} // namespace

// ------------------------------------------------------------
// Parsing helpers
// ------------------------------------------------------------

std::vector<int> Day10::parse_list(std::string_view s) {
    std::vector<int> out;
    if (s.size() < 2)
        return out;

    const auto inner = s.substr(1, s.size() - 2);
    std::string token;
    std::istringstream ss(std::string{inner});

    while (std::getline(ss, token, ',')) {
        out.push_back(core::parse_integer<int>(token));
    }
    if (!inner.empty() && inner.back() == ',')
        throw std::invalid_argument("Trailing comma in machine list");
    return out;
}

void Day10::set_input(const std::vector<std::string>& lines) {
    machines.clear();

    for (const auto& line : lines) {
        if (line.empty())
            continue;

        // lights
        auto lb = line.find('[');
        auto rb = line.find(']');
        if (lb == std::string::npos || rb == std::string::npos || rb <= lb)
            throw std::invalid_argument("Missing machine lights");

        std::vector<int> lights;
        for (char c : line.substr(lb + 1, rb - lb - 1)) {
            if (c != '#' && c != '.')
                throw std::invalid_argument("Invalid light state");
            lights.push_back(c == '#' ? 1 : 0);
        }

        // joltage
        std::vector<int> joltage;
        auto lcb = line.find('{');
        auto rcb = line.find('}');
        if (lcb == std::string::npos || rcb == std::string::npos || lcb <= rb || rcb <= lcb)
            throw std::invalid_argument("Missing machine joltage");
        joltage = parse_list(line.substr(lcb, rcb - lcb + 1));

        // buttons
        std::vector<std::vector<int>> buttons;
        auto mid = core::trim(std::string_view(line).substr(rb + 1, lcb - rb - 1));

        std::size_t pos = 0;
        while (!mid.empty()) {
            if (mid.front() != '(')
                throw std::invalid_argument("Expected a machine button");
            auto end = mid.find(')', pos);
            if (end == std::string::npos)
                throw std::invalid_argument("Unclosed machine button");
            buttons.push_back(parse_list(mid.substr(pos, end - pos + 1)));
            mid = core::trim(mid.substr(end + 1));
        }

        if (!core::trim(std::string_view(line).substr(0, lb)).empty() ||
            !core::trim(std::string_view(line).substr(rcb + 1)).empty())
            throw std::invalid_argument("Unexpected text around machine");
        if (lights.empty() || lights.size() != joltage.size() ||
            std::ranges::any_of(joltage, [](int value) { return value < 0; }))
            throw std::invalid_argument("Invalid machine targets");
        for (auto& button : buttons) {
            std::ranges::sort(button);
            if (std::ranges::any_of(button,
                                    [&](int index) {
                                        return index < 0 ||
                                               index >= static_cast<int>(lights.size());
                                    }) ||
                std::adjacent_find(button.begin(), button.end()) != button.end())
                throw std::invalid_argument("Invalid machine button index");
        }
        // Repeated or empty buttons cannot improve a minimum-press solution.
        std::erase_if(buttons, [](const auto& button) { return button.empty(); });
        std::ranges::sort(buttons);
        buttons.erase(std::unique(buttons.begin(), buttons.end()), buttons.end());
        machines.push_back({std::move(lights), std::move(joltage), std::move(buttons)});
    }
}

// ------------------------------------------------------------
// Part 1 — GF(2) Gaussian elimination
// ------------------------------------------------------------

int Day10::solve_lights(const MachineData& m) {
    int N = static_cast<int>(m.targetLights.size());
    int M = static_cast<int>(m.buttons.size());

    std::vector<std::vector<int>> mat(N, std::vector<int>(M + 1, 0));
    for (int i = 0; i < N; ++i)
        mat[i][M] = m.targetLights[i];

    for (int j = 0; j < M; ++j)
        for (int idx : m.buttons[j])
            if (idx < N)
                mat[idx][j] = 1;

    int row = 0;
    std::vector<int> pivotCol(M, -1);

    for (int col = 0; col < M && row < N; ++col) {
        int sel = -1;
        for (int r = row; r < N; ++r) {
            if (mat[r][col]) {
                sel = r;
                break;
            }
        }
        if (sel == -1)
            continue;

        std::swap(mat[row], mat[sel]);
        pivotCol[col] = row;

        for (int r = 0; r < N; ++r) {
            if (r != row && mat[r][col]) {
                for (int k = col; k <= M; ++k)
                    mat[r][k] ^= mat[row][k];
            }
        }
        row++;
    }

    for (int r = row; r < N; ++r) {
        if (mat[r][M] != 0)
            throw std::runtime_error("Unreachable light target");
    }

    std::vector<int> freeVars;
    for (int c = 0; c < M; ++c)
        if (pivotCol[c] == -1)
            freeVars.push_back(c);

    int best = M + 1;
    std::vector<int> x(M, 0);
    const auto search = [&](auto&& self, std::size_t index, int presses) -> void {
        if (presses >= best)
            return;
        if (index < freeVars.size()) {
            const int column = freeVars[index];
            x[column] = 0;
            self(self, index + 1, presses);
            x[column] = 1;
            self(self, index + 1, presses + 1);
            return;
        }
        for (int c = M - 1; c >= 0; --c) {
            if (pivotCol[c] == -1)
                continue;
            const int r = pivotCol[c];
            int value = mat[r][M];
            for (int k = c + 1; k < M; ++k)
                value ^= mat[r][k] & x[k];
            x[c] = value;
            presses += value;
        }
        best = std::min(best, presses);
    };
    search(search, 0, 0);

    return best;
}

// ------------------------------------------------------------
// Part 2 — exact integer recursion on the binary digits of press counts
// ------------------------------------------------------------

std::int64_t Day10::solve_joltage(const MachineData& machine) {
    struct VectorHash {
        std::size_t operator()(const std::vector<int>& values) const noexcept {
            std::size_t hash = 0;
            for (int value : values)
                hash ^= std::hash<int>{}(value) + 0x9e3779b9U + (hash << 6) + (hash >> 2);
            return hash;
        }
    };
    struct Choice {
        std::vector<int> increments;
        int presses;
    };

    const auto counters = machine.targetJoltage.size();
    std::unordered_map<std::vector<int>, std::vector<Choice>, VectorHash> choices_by_parity;
    std::vector<int> increments(counters, 0);
    const auto enumerate = [&](auto&& self, std::size_t button, int presses) -> void {
        if (button == machine.buttons.size()) {
            auto parity = increments;
            for (auto& value : parity)
                value %= 2;
            choices_by_parity[parity].push_back({increments, presses});
            return;
        }
        self(self, button + 1, presses);
        for (int counter : machine.buttons[button])
            ++increments[counter];
        self(self, button + 1, presses + 1);
        for (int counter : machine.buttons[button])
            --increments[counter];
    };
    enumerate(enumerate, 0, 0);

    using Cost = std::optional<std::int64_t>;
    std::unordered_map<std::vector<int>, Cost, VectorHash> memo;
    const auto minimum_presses = [&](auto&& self, const std::vector<int>& target) -> Cost {
        if (std::ranges::all_of(target, [](int value) { return value == 0; }))
            return 0;
        if (const auto cached = memo.find(target); cached != memo.end())
            return cached->second;

        auto parity = target;
        for (auto& value : parity)
            value %= 2;
        const auto choices = choices_by_parity.find(parity);
        Cost best;
        if (choices != choices_by_parity.end()) {
            for (const auto& choice : choices->second) {
                std::vector<int> half(counters);
                bool feasible = true;
                for (std::size_t i = 0; i < counters; ++i) {
                    if (choice.increments[i] > target[i]) {
                        feasible = false;
                        break;
                    }
                    half[i] = (target[i] - choice.increments[i]) / 2;
                }
                if (!feasible)
                    continue;
                // Each press adds at most one to any counter.
                const auto lower_bound = choice.presses + 2 * std::int64_t{std::ranges::max(half)};
                if (best && lower_bound >= *best)
                    continue;
                if (const auto remaining = self(self, half)) {
                    const auto total = choice.presses + 2 * *remaining;
                    if (!best || total < *best)
                        best = total;
                }
            }
        }
        memo.emplace(target, best);
        return best;
    };

    // Any press vector is uniquely x = odd + 2 * rest. Matching target parity
    // makes (target - A * odd) / 2 an exact, smaller integer subproblem.
    const auto result = minimum_presses(minimum_presses, machine.targetJoltage);
    if (!result)
        throw std::runtime_error("Unreachable joltage target");
    return *result;
}

// ------------------------------------------------------------
// Day interface
// ------------------------------------------------------------

std::string Day10::part1() {
    const std::int64_t total =
        core::parallel_sum_indexed(machines.size(), [&](std::size_t i) -> std::int64_t {
            const auto& m = machines[i];
            if (m.targetLights.empty())
                return 0;
            return static_cast<std::int64_t>(solve_lights(m));
        });

    return std::to_string(total);
}

std::string Day10::part2() {
    const std::int64_t total =
        core::parallel_sum_indexed(machines.size(), [&](std::size_t i) -> std::int64_t {
            const auto& m = machines[i];
            if (m.targetJoltage.empty())
                return 0;
            return static_cast<std::int64_t>(solve_joltage(m));
        });

    return std::to_string(total);
}
