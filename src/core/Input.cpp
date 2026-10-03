#include "core/Input.h"

#include <fstream>
#include <stdexcept>
#include <utility>

namespace core {

std::vector<std::string> RgusReadLines(std::istream& inPuzzle) {
    std::vector<std::string> rgusLines;
    for (std::string usLine; std::getline(inPuzzle, usLine);) {
        if (!usLine.empty() && usLine.back() == '\r')
            usLine.pop_back();
        rgusLines.push_back(std::move(usLine));
    }
    if (inPuzzle.bad() || (inPuzzle.fail() && !inPuzzle.eof())) {
        throw std::runtime_error("Failed to read input");
    }
    return rgusLines;
}

std::vector<std::string> RgusReadInput(int idDay, const std::filesystem::path& pathDirectory) {
    const auto pathInput = pathDirectory / ("day" + std::string(idDay < 10 ? "0" : "") +
                                            std::to_string(idDay) + ".txt");
    std::ifstream inPuzzle(pathInput);
    if (!inPuzzle)
        throw std::runtime_error("Could not open input file: " +
                                 std::filesystem::absolute(pathInput).string());
    return RgusReadLines(inPuzzle);
}

} // namespace core
