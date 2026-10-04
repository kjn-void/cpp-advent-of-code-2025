#include "Registry.h"
#include "Solution.h"

#include <algorithm>

Registry& Registry::instance() {
    static Registry registry;
    return registry;
}

void Registry::register_day(int day, Factory factory) {
    factories_[day] = std::move(factory);
}

std::unique_ptr<Solution> Registry::make(int day) const {
    if (auto factory_entry = factories_.find(day); factory_entry != factories_.end()) {
        return factory_entry->second();
    }
    return nullptr;
}

std::vector<int> Registry::implemented_days() const {
    std::vector<int> days;
    days.reserve(factories_.size());
    for (const auto& [day, unused_factory] : factories_) {
        days.push_back(day);
    }
    std::ranges::sort(days);
    return days;
}
