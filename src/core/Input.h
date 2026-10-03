#pragma once

#include <filesystem>
#include <istream>
#include <string>
#include <vector>

namespace core {

std::vector<std::string> RgusReadLines(std::istream& inPuzzle);
std::vector<std::string> RgusReadInput(int idDay,
                                       const std::filesystem::path& pathDirectory = "input");

} // namespace core
