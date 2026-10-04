#include "test_utils.h"
#include "core/Input.h"

#include <sstream>

std::vector<std::string> SplitLines(const std::string& stringText) {
    std::istringstream istringstreamInput(stringText);
    return core::ReadLines(istringstreamInput);
}
