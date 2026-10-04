#pragma once

#include "core/Registry.h"
#include "core/Solution.h"

#include <concepts>
#include <memory>

namespace core {

template <std::derived_from<Solution> DAY_SOLVER> struct DayRegistration {
    explicit DayRegistration(int iDay) {
        Registry::Instance().RegisterDay(iDay, [] { return std::make_unique<DAY_SOLVER>(); });
    }
};

} // namespace core
