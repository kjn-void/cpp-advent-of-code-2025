#include "test_utils.h"
#include "core/Input.h"

#include <sstream>

std::vector<std::string> split_lines(const std::string& text) {
    std::istringstream input(text);
    return core::read_lines(input);
}
