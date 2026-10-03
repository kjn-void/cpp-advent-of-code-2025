#pragma once

#include "core/Registry.h"
#include "core/Solution.h"

#include <concepts>
#include <memory>

namespace core {

template <std::derived_from<Slv> SlvDay> struct Drg {
    explicit Drg(int idDay) {
        Regy::RegyInstance().RegisterDay(idDay, [] { return std::make_unique<SlvDay>(); });
    }
};

} // namespace core
