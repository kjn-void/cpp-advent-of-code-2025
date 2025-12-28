#include "Registry.h"
#include "Solution.h"

#include <algorithm>

Registry& Registry::instance() {
    static Registry inst;
    return inst;
}

void Registry::register_day(int day, Factory factory) {
    factories_[day] = std::move(factory);
}

std::unique_ptr<Solution> Registry::make(int day) const {
    if (auto it = factories_.find(day); it != factories_.end()) {
        return it->second();
    }
    return nullptr;
}

std::vector<int> Registry::implemented_days() const {
    std::vector<int> days;
    days.reserve(factories_.size());
    for (const auto& [day, _] : factories_) {
        days.push_back(day);
    }
    std::sort(days.begin(), days.end());
    return days;
}