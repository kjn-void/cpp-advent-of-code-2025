#include "days/day06.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <stdexcept>

// -----------------------------------------------------------------------------
// Registration
// -----------------------------------------------------------------------------
namespace {
const core::DayRegistration<Day06> registration{6};
} // namespace

// -----------------------------------------------------------------------------
// Input
// -----------------------------------------------------------------------------

void Day06::set_input(const std::vector<std::string>& input_lines) {
    worksheet_ = input_lines;

    // normalize width
    column_count_ = 0;
    for (const auto& input_row : worksheet_) {
        column_count_ = std::max(column_count_, static_cast<int>(input_row.size()));
    }
    for (auto& input_row : worksheet_) {
        if (static_cast<int>(input_row.size()) < column_count_) {
            input_row.append(column_count_ - input_row.size(), ' ');
        }
    }

    row_count_ = static_cast<int>(worksheet_.size());
}

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

std::vector<Day06::ProblemColumns> Day06::find_problems() const {
    std::vector<bool> blank_columns(column_count_, true);

    for (int column = 0; column < column_count_; ++column) {
        for (int row = 0; row < row_count_; ++row) {
            if (worksheet_[row][column] != ' ') {
                blank_columns[column] = false;
                break;
            }
        }
    }

    std::vector<ProblemColumns> problems;
    bool in_problem = false;
    int first_column = 0;

    for (int column = 0; column < column_count_; ++column) {
        if (!blank_columns[column]) {
            if (!in_problem) {
                in_problem = true;
                first_column = column;
            }
        } else if (in_problem) {
            problems.push_back({first_column, column - 1});
            in_problem = false;
        }
    }

    if (in_problem) {
        problems.push_back({first_column, column_count_ - 1});
    }

    return problems;
}

char Day06::problem_operator(const ProblemColumns& problem) const {
    const auto& input_row = worksheet_[row_count_ - 1];
    for (int column = problem.first_column; column <= problem.last_column; ++column) {
        if (input_row[column] == '+' || input_row[column] == '*') {
            return input_row[column];
        }
    }
    throw std::invalid_argument("Missing worksheet operator");
}

// -----------------------------------------------------------------------------
// Extractors
// -----------------------------------------------------------------------------

std::vector<std::int64_t> Day06::read_numbers_by_row(const ProblemColumns& problem) const {
    std::vector<std::int64_t> numbers;
    numbers.reserve(row_count_);

    for (int row = 0; row < row_count_ - 1; ++row) {
        std::string number_text = worksheet_[row].substr(
            problem.first_column, problem.last_column - problem.first_column + 1);
        number_text.erase(0, number_text.find_first_not_of(' '));
        number_text.erase(number_text.find_last_not_of(' ') + 1);
        numbers.push_back(core::parse_integer<std::int64_t>(number_text));
    }
    return numbers;
}

std::vector<std::int64_t> Day06::read_numbers_by_column(const ProblemColumns& problem) const {
    std::vector<std::int64_t> numbers;
    numbers.reserve(problem.last_column - problem.first_column + 1);

    for (int column = problem.first_column; column <= problem.last_column; ++column) {
        std::string number_text;
        for (int row = 0; row < row_count_ - 1; ++row) {
            char digit = worksheet_[row][column];
            if (digit != ' ')
                number_text.push_back(digit);
        }
        numbers.push_back(core::parse_integer<std::int64_t>(number_text));
    }
    return numbers;
}

// -----------------------------------------------------------------------------
// Evaluation
// -----------------------------------------------------------------------------

std::int64_t Day06::evaluate_problem(std::span<const std::int64_t> numbers, char operation) {
    if (operation == '+') {
        std::int64_t sum = 0;
        for (auto number : numbers)
            sum += number;
        return sum;
    }

    std::int64_t product = 1;
    for (auto number : numbers)
        product *= number;
    return product;
}

template <typename Function> std::int64_t Day06::grand_total(Function&& read_numbers) const {
    std::int64_t total = 0;

    for (const auto& problem : find_problems()) {
        auto numbers = read_numbers(problem);
        char operation = problem_operator(problem);
        total += evaluate_problem(numbers, operation);
    }
    return total;
}

// -----------------------------------------------------------------------------
// Parts
// -----------------------------------------------------------------------------

std::string Day06::part1() {
    return std::to_string(grand_total(
        [this](const ProblemColumns& problem) { return read_numbers_by_row(problem); }));
}

std::string Day06::part2() {
    return std::to_string(grand_total(
        [this](const ProblemColumns& problem) { return read_numbers_by_column(problem); }));
}
