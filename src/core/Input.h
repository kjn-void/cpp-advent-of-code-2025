#pragma once

#include <filesystem>
#include <istream>
#include <string>
#include <vector>

namespace core {

std::vector<std::string> ReadLines(std::istream& istreamInput);
std::vector<std::string> ReadInput(int iDay,
                                   const std::filesystem::path& pathInputDirectory = "input");

} // namespace core
