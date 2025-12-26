#include "core/Solution.h"
#include "core/Registry.h"

#include <string>
#include <vector>
#include <cstdlib>

class Day01 final : public Solution {
public:
    void set_input(const std::vector<std::string>& lines) override {
        moves.clear();

        for (const auto& line : lines) {
            if (line.empty()) continue;

            char dir = line[0];
            int val = std::stoi(line.substr(1));

            moves.push_back(dir == 'L' ? -val : val);
        }
    }

    std::string part1() override {
        int pos = 50;
        int count_zero = 0;

        for (int delta : moves) {
            pos = mod100(pos + delta);
            if (pos == 0) {
                ++count_zero;
            }
        }

        return std::to_string(count_zero);
    }

    std::string part2() override {
        int pos = 50;
        int count_zero = 0;

        for (int delta : moves) {
            int step = delta < 0 ? -1 : 1;

            for (int moved = 0; moved != delta; moved += step) {
                pos += step;
                if (pos < 0) pos += 100;
                if (pos >= 100) pos -= 100;

                if (pos == 0) {
                    ++count_zero;
                }
            }
        }

        return std::to_string(count_zero);
    }

private:
    static int mod100(int n) {
        n %= 100;
        if (n < 0) n += 100;
        return n;
    }

    std::vector<int> moves;
};

// 🔌 Registration (static init, like Go)
static bool registered = [] {
    Registry::instance().register_day(
        1,
        [] { return std::make_unique<Day01>(); }
    );
    return true;
}();