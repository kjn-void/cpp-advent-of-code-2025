#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

struct Slv; // forward declaration

class Regy {
  public:
    using Fac = std::function<std::unique_ptr<Slv>()>;

    static Regy& RegyInstance();

    void RegisterDay(int idDay, Fac facDay);
    std::unique_ptr<Slv> PslvMake(int idDay) const;

    std::vector<int> RgidImplementedDays() const;

  private:
    Regy() = default;

    std::unordered_map<int, Fac> mpidfacDay_;
};
