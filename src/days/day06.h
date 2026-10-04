#pragma once

#include <span>

#include "core/Solution.h"
#include <cstdint>
#include <string>
#include <vector>

class Day06 final : public Solution {
  public:
    void SetInput(const std::vector<std::string>& vectorInputLines) override;
    std::string Part1() override;
    std::string Part2() override;

  private:
    struct ProblemColumns {
        int m_iFirstColumn;
        int m_iLastColumn;
    };

    std::vector<std::string> m_vectorWorksheet;
    int m_iRowCount = 0;
    int m_iColumnCount = 0;

    std::vector<ProblemColumns> FindProblems() const;
    char ProblemOperation(const ProblemColumns& problemcolumns) const;

    std::vector<std::int64_t> ReadNumbersByRow(const ProblemColumns& problemcolumns) const;
    std::vector<std::int64_t> ReadNumbersByColumn(const ProblemColumns& problemcolumns) const;

    template <typename NUMBER_READER> std::int64_t GrandTotal(NUMBER_READER&& numberreader) const;

    static std::int64_t EvaluateProblem(std::span<const std::int64_t> spanNumbers, char iOperation);
};
