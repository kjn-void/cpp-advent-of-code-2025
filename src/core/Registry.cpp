#include "Registry.h"
#include "Solution.h"

#include <algorithm>

Regy& Regy::RegyInstance() {
    static Regy regy;
    return regy;
}

void Regy::RegisterDay(int idDay, Fac facDay) {
    mpidfacDay_[idDay] = std::move(facDay);
}

std::unique_ptr<Slv> Regy::PslvMake(int idDay) const {
    if (auto itFactory = mpidfacDay_.find(idDay); itFactory != mpidfacDay_.end()) {
        return itFactory->second();
    }
    return nullptr;
}

std::vector<int> Regy::RgidImplementedDays() const {
    std::vector<int> rgidDay;
    rgidDay.reserve(mpidfacDay_.size());
    for (const auto& [idDay, facIgnored] : mpidfacDay_) {
        rgidDay.push_back(idDay);
    }
    std::ranges::sort(rgidDay);
    return rgidDay;
}
