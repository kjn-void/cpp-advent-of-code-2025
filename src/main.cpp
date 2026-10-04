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

    int exit_code = 0;

    for (int argument_index = 1; argument_index < argc; argument_index++) {
        int day = 0;
        try {
            day = core::parse_integer<int>(argv[argument_index]);
        } catch (const std::exception& error) {
            std::cerr << "Invalid day argument '" << argv[argument_index] << "': " << error.what()
                      << "\n";
            exit_code = 1;
            continue;
        }

        auto solver = Registry::instance().make(day);
        if (!solver) {
            std::cerr << "Day " << day << " not implemented\n";
            exit_code = 1;
            continue;
        }

        try {
            auto input_lines = core::read_input(day);
            solver->set_input(input_lines);

            const auto part1_answer = solver->part1();
            const auto part2_answer = solver->part2();
            std::cout << "Day " << day << "\n";
            std::cout << "  Part 1: " << part1_answer << "\n";
            std::cout << "  Part 2: " << part2_answer << "\n";
        } catch (const std::exception& error) {
            std::cerr << "Day " << day << " failed: " << error.what() << "\n";
            exit_code = 1;
        }
    }

    return exit_code;
}
