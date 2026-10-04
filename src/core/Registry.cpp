#include "Registry.h"
#include "Solution.h"

#include <algorithm>

Registry& Registry::Instance() {
    static Registry registry;
    return registry;
}

void Registry::RegisterDay(int iDay, Factory functionFactory) {
    m_mapFactories[iDay] = std::move(functionFactory);
}

std::unique_ptr<Solution> Registry::Make(int iDay) const {
    if (auto itFactoryEntry = m_mapFactories.find(iDay); itFactoryEntry != m_mapFactories.end()) {
        return itFactoryEntry->second();
    }
    return nullptr;
}

std::vector<int> Registry::ImplementedDays() const {
    std::vector<int> vectorDays;
    vectorDays.reserve(m_mapFactories.size());
    for (const auto& [iDay, functionUnusedFactory] : m_mapFactories) {
        vectorDays.push_back(iDay);
    }
    std::ranges::sort(vectorDays);
    return vectorDays;
}
