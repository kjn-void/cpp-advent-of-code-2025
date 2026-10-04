#include "core/Input.h"

#include <fstream>
#include <stdexcept>
#include <utility>

namespace core {

std::vector<std::string> read_lines(std::istream& input) {
    std::vector<std::string> input_lines;
    for (std::string line; std::getline(input, line);) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        input_lines.push_back(std::move(line));
    }
    if (input.bad() || (input.fail() && !input.eof())) {
        throw std::runtime_error("Failed to read input");
    }
    return input_lines;
}

std::vector<std::string> read_input(int day, const std::filesystem::path& input_directory) {
    const auto input_path =
        input_directory / ("day" + std::string(day < 10 ? "0" : "") + std::to_string(day) + ".txt");
    std::ifstream input(input_path);
    if (!input)
        throw std::runtime_error("Could not open input file: " +
                                 std::filesystem::absolute(input_path).string());
    return read_lines(input);
}

} // namespace core
