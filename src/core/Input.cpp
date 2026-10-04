#include "core/Input.h"

#include <fstream>
#include <stdexcept>
#include <utility>

namespace core {

std::vector<std::string> ReadLines(std::istream& istreamInput) {
    std::vector<std::string> vectorInputLines;
    for (std::string stringLine; std::getline(istreamInput, stringLine);) {
        if (!stringLine.empty() && stringLine.back() == '\r')
            stringLine.pop_back();
        vectorInputLines.push_back(std::move(stringLine));
    }
    if (istreamInput.bad() || (istreamInput.fail() && !istreamInput.eof())) {
        throw std::runtime_error("Failed to read input");
    }
    return vectorInputLines;
}

std::vector<std::string> ReadInput(int iDay, const std::filesystem::path& pathInputDirectory) {
    const auto pathInput = pathInputDirectory / ("day" + std::string(iDay < 10 ? "0" : "") +
                                                 std::to_string(iDay) + ".txt");
    std::ifstream ifstreamInput(pathInput);
    if (!ifstreamInput)
        throw std::runtime_error("Could not open input file: " +
                                 std::filesystem::absolute(pathInput).string());
    return ReadLines(ifstreamInput);
}

} // namespace core
