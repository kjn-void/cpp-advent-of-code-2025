#include "test_utils.h"
#include "core/Input.h"

#include <sstream>

std::vector<std::string> RgusSplitLines(const std::string& usText) {
    std::istringstream inLines(usText);
    return core::RgusReadLines(inLines);
}
