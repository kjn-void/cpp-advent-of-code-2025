#include "core/Parallel.h"
#include "core/Registry.h"
#include "days/day10.h"

#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>

using namespace std;

// 🔌 Registration
static bool registered = [] {
    Registry::instance().register_day(
        10, [] { return make_unique<Day10>(); }
    );
    return true;
}();

// ------------------------------------------------------------
// Parsing helpers
// ------------------------------------------------------------

vector<int> Day10::parse_list(const string& s) {
    vector<int> out;
    if (s.size() < 2) return out;

    string inner = s.substr(1, s.size() - 2);
    string token;
    stringstream ss(inner);

    while (getline(ss, token, ',')) {
        out.push_back(stoi(token));
    }
    return out;
}

void Day10::set_input(const vector<string>& lines) {
    machines.clear();

    for (auto line : lines) {
        if (line.empty()) continue;

        // lights
        auto lb = line.find('[');
        auto rb = line.find(']');
        if (lb == string::npos || rb == string::npos) continue;

        vector<int> lights;
        for (char c : line.substr(lb + 1, rb - lb - 1)) {
            lights.push_back(c == '#' ? 1 : 0);
        }

        // joltage
        vector<int> joltage;
        auto lcb = line.find('{');
        auto rcb = line.find('}');
        if (lcb != string::npos && rcb != string::npos) {
            joltage = parse_list(line.substr(lcb, rcb - lcb + 1));
        }

        // buttons
        vector<vector<int>> buttons;
        string mid = line.substr(rb + 1, lcb == string::npos ? string::npos : lcb - rb - 1);

        size_t pos = 0;
        while ((pos = mid.find('(')) != string::npos) {
            auto end = mid.find(')', pos);
            buttons.push_back(parse_list(mid.substr(pos, end - pos + 1)));
            mid = mid.substr(end + 1);
        }

        machines.push_back({lights, joltage, buttons});
    }
}

// ------------------------------------------------------------
// Part 1 — GF(2) Gaussian elimination
// ------------------------------------------------------------

int Day10::solve_lights(const MachineData& m) {
    int N = (int)m.targetLights.size();
    int M = (int)m.buttons.size();

    vector<vector<int>> mat(N, vector<int>(M + 1, 0));
    for (int i = 0; i < N; ++i) mat[i][M] = m.targetLights[i];

    for (int j = 0; j < M; ++j)
        for (int idx : m.buttons[j])
            if (idx < N) mat[idx][j] = 1;

    int row = 0;
    vector<int> pivotCol(M, -1);

    for (int col = 0; col < M && row < N; ++col) {
        int sel = -1;
        for (int r = row; r < N; ++r) {
            if (mat[r][col]) { sel = r; break; }
        }
        if (sel == -1) continue;

        swap(mat[row], mat[sel]);
        pivotCol[col] = row;

        for (int r = 0; r < N; ++r) {
            if (r != row && mat[r][col]) {
                for (int k = col; k <= M; ++k)
                    mat[r][k] ^= mat[row][k];
            }
        }
        row++;
    }

    vector<int> freeVars;
    for (int c = 0; c < M; ++c)
        if (pivotCol[c] == -1)
            freeVars.push_back(c);

    int best = M + 1;
    int maxMask = 1 << freeVars.size();

    for (int mask = 0; mask < maxMask; ++mask) {
        vector<int> x(M, 0);

        for (int i = 0; i < (int)freeVars.size(); ++i)
            if (mask & (1 << i))
                x[freeVars[i]] = 1;

        for (int c = M - 1; c >= 0; --c) {
            if (pivotCol[c] != -1) {
                int r = pivotCol[c];
                int v = mat[r][M];
                for (int k = c + 1; k < M; ++k)
                    if (mat[r][k]) v ^= x[k];
                x[c] = v;
            }
        }

        int sum = 0;
        for (int v : x) sum += v;
        best = min(best, sum);
    }

    return best;
}

// ------------------------------------------------------------
// Part 2 — real RREF + integer DFS
// ------------------------------------------------------------

int Day10::solve_joltage(const MachineData& m) {
    int N = (int)m.targetJoltage.size();
    int M = (int)m.buttons.size();

    vector<vector<double>> mat(N, vector<double>(M + 1, 0.0));
    for (int i = 0; i < N; ++i) mat[i][M] = m.targetJoltage[i];

    for (int j = 0; j < M; ++j)
        for (int idx : m.buttons[j])
            if (idx < N) mat[idx][j] = 1.0;

    int row = 0;
    vector<int> pivotCol(M, -1);

    for (int col = 0; col < M && row < N; ++col) {
        int sel = -1;
        for (int r = row; r < N; ++r)
            if (fabs(mat[r][col]) > 1e-9) { sel = r; break; }
        if (sel == -1) continue;

        swap(mat[row], mat[sel]);
        pivotCol[col] = row;

        double div = mat[row][col];
        for (int k = col; k <= M; ++k) mat[row][k] /= div;

        for (int r = 0; r < N; ++r) {
            if (r == row) continue;
            double f = mat[r][col];
            if (fabs(f) < 1e-9) continue;
            for (int k = col; k <= M; ++k)
                mat[r][k] -= f * mat[row][k];
        }
        row++;
    }

    vector<int> freeVars;
    for (int c = 0; c < M; ++c)
        if (pivotCol[c] == -1)
            freeVars.push_back(c);

    int maxTarget = 0;
    for (int v : m.targetJoltage)
        maxTarget = max(maxTarget, v);

    int best = numeric_limits<int>::max();
    vector<double> x(M, 0.0);

    function<void(int,int)> dfs = [&](int idx, int sum) {
        if (sum >= best) return;
        if (idx == (int)freeVars.size()) {
            int total = sum;
            for (int c = 0; c < M; ++c) {
                if (pivotCol[c] != -1) {
                    int r = pivotCol[c];
                    double v = mat[r][M];
                    for (int k = c + 1; k < M; ++k)
                        v -= mat[r][k] * x[k];
                    if (v < -1e-6 || fabs(v - round(v)) > 1e-6) return;
                    total += (int)round(v);
                }
            }
            best = min(best, total);
            return;
        }

        int c = freeVars[idx];
        for (int v = 0; v <= maxTarget + 1; ++v) {
            x[c] = v;
            dfs(idx + 1, sum + v);
        }
    };

    dfs(0, 0);
    return best;
}

// ------------------------------------------------------------
// Day interface
// ------------------------------------------------------------

std::string Day10::part1() {
    const int64_t total = core::parallel_sum_indexed(
        machines.size(),
        [&](std::size_t i) -> int64_t {
            const auto& m = machines[i];
            if (m.targetLights.empty()) return 0;
            return static_cast<int64_t>(solve_lights(m));
        }
    );

    return std::to_string(total);
}

std::string Day10::part2() {
    const int64_t total = core::parallel_sum_indexed(
        machines.size(),
        [&](std::size_t i) -> int64_t {
            const auto& m = machines[i];
            if (m.targetJoltage.empty()) return 0;
            return static_cast<int64_t>(solve_joltage(m));
        }
    );

    return std::to_string(total);
}