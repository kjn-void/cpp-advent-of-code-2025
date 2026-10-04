#pragma once

#include <span>

#include "core/Solution.h"
#include <cstdint>
#include <string>
#include <vector>

class Day06 final : public Solution {
  public:
    void set_input(const std::vector<std::string>& input_lines) override;
    std::string part1() override;
    std::string part2() override;

  private:
    struct ProblemColumns {
        int first_column;
        int last_column;
    };

    std::vector<std::string> worksheet_;
    int row_count_ = 0;
    int column_count_ = 0;

    std::vector<ProblemColumns> find_problems() const;
    char problem_operator(const ProblemColumns& problem) const;

    std::vector<std::int64_t> read_numbers_by_row(const ProblemColumns& problem) const;
    std::vector<std::int64_t> read_numbers_by_column(const ProblemColumns& problem) const;

    template <typename Function> std::int64_t grand_total(Function&& read_numbers) const;

    static std::int64_t evaluate_problem(std::span<const std::int64_t> numbers, char operation);
};
