#include "core/Solution.h"
#include "core/Registry.h"
#include <fstream>
#include <iostream>
#include <filesystem>

static std::vector<std::string> read_input(int day) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "input/day%02d.txt", day);
    std::ifstream f(buf);

    if (!f) {
        std::filesystem::path cwd = std::filesystem::current_path();
		std::cerr << "Could not open input file: " << buf << " cwd is " << cwd << "\n";
        throw std::runtime_error("Missing input file");
    }

    std::vector<std::string> lines;
    std::string s;
    while (std::getline(f, s)) lines.push_back(s);
    return lines;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: aoc2025 DAY\n";
        return 1;
    }

    int day = std::stoi(argv[1]);
    auto solver = Registry::instance().make(day);
    if (!solver) {
        std::cerr << "Day " << day << " not implemented\n";
        return 1;
    }

    auto lines = read_input(day);
    solver->set_input(lines);

    std::cout << "Part 1: " << solver->part1() << "\n";
    std::cout << "Part 2: " << solver->part2() << "\n";
}