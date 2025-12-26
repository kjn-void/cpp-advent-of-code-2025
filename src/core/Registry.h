#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

struct Solution; // forward declaration

class Registry {
public:
    using Factory = std::function<std::unique_ptr<Solution>()>;

    static Registry& instance();

    void register_day(int day, Factory factory);
    std::unique_ptr<Solution> make(int day) const;

    std::vector<int> implemented_days() const;

private:
    Registry() = default;

    std::unordered_map<int, Factory> factories_;
};