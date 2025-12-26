#include "test_utils.h"

#include <sstream>

std::vector<std::string> splitLines(const std::string& s) {
    std::vector<std::string> out;
    std::istringstream iss(s);
    std::string line;

    while (std::getline(iss, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (!line.empty()) {
            out.push_back(line);
        }
    }
    return out;
}