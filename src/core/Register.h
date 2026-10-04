#pragma once

#include "core/Registry.h"
#include "core/Solution.h"

#include <concepts>
#include <memory>

namespace core {

template <std::derived_from<Solution> DaySolver> struct DayRegistration {
    explicit DayRegistration(int day) {
        Registry::instance().register_day(day, [] { return std::make_unique<DaySolver>(); });
    }
};

} // namespace core
