#include "core/Input.h"
#include "core/Parse.h"
#include "core/Registry.h"
#include "core/Solution.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: aoc2025 DAY [DAY ...]\n";
        return 1;
    }

    int exitCode = 0;

    for (int argIndex = 1; argIndex < argc; argIndex++) {
        int day = 0;
        try {
            day = core::parse_integer<int>(argv[argIndex]);
        } catch (const std::exception& ex) {
            std::cerr << "Invalid day argument '" << argv[argIndex] << "': " << ex.what() << "\n";
            exitCode = 1;
            continue;
        }

        auto solver = Registry::instance().make(day);
        if (!solver) {
            std::cerr << "Day " << day << " not implemented\n";
            exitCode = 1;
            continue;
        }

        try {
            auto lines = core::read_input(day);
            solver->set_input(lines);

            const auto part1 = solver->part1();
            const auto part2 = solver->part2();
            std::cout << "Day " << day << "\n";
            std::cout << "  Part 1: " << part1 << "\n";
            std::cout << "  Part 2: " << part2 << "\n";
        } catch (const std::exception& ex) {
            std::cerr << "Day " << day << " failed: " << ex.what() << "\n";
            exitCode = 1;
        }
    }

    return exitCode;
}
