#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

struct Solution; // forward declaration

class Registry {
  public:
    using Factory = std::function<std::unique_ptr<Solution>()>;

    static Registry& Instance();

    void RegisterDay(int iDay, Factory functionFactory);
    std::unique_ptr<Solution> Make(int iDay) const;

    std::vector<int> ImplementedDays() const;

  private:
    Registry() = default;

    std::unordered_map<int, Factory> m_mapFactories;
};
