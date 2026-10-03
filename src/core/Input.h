#pragma once

#include <filesystem>
#include <istream>
#include <string>
#include <vector>

namespace core {

std::vector<std::string> read_lines(std::istream& input);
std::vector<std::string> read_input(int day, const std::filesystem::path& directory = "input");

} // namespace core
