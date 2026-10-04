#include "days/day06.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <stdexcept>

// -----------------------------------------------------------------------------
// Registration
// -----------------------------------------------------------------------------
namespace {
const core::DayRegistration<Day06> dayregistration{6};
} // namespace

// -----------------------------------------------------------------------------
// Input
// -----------------------------------------------------------------------------

void Day06::SetInput(const std::vector<std::string>& vectorInputLines) {
    m_vectorWorksheet = vectorInputLines;

    // normalize width
    m_iColumnCount = 0;
    for (const auto& stringInputRow : m_vectorWorksheet) {
        m_iColumnCount = std::max(m_iColumnCount, static_cast<int>(stringInputRow.size()));
    }
    for (auto& stringInputRow : m_vectorWorksheet) {
        if (static_cast<int>(stringInputRow.size()) < m_iColumnCount) {
            stringInputRow.append(m_iColumnCount - stringInputRow.size(), ' ');
        }
    }

    m_iRowCount = static_cast<int>(m_vectorWorksheet.size());
}

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

std::vector<Day06::ProblemColumns> Day06::FindProblems() const {
    std::vector<bool> vectorIsBlankColumn(m_iColumnCount, true);

    for (int iColumn = 0; iColumn < m_iColumnCount; ++iColumn) {
        for (int iRow = 0; iRow < m_iRowCount; ++iRow) {
            if (m_vectorWorksheet[iRow][iColumn] != ' ') {
                vectorIsBlankColumn[iColumn] = false;
                break;
            }
        }
    }

    std::vector<ProblemColumns> vectorProblems;
    bool bInProblem = false;
    int iFirstColumn = 0;

    for (int iColumn = 0; iColumn < m_iColumnCount; ++iColumn) {
        if (!vectorIsBlankColumn[iColumn]) {
            if (!bInProblem) {
                bInProblem = true;
                iFirstColumn = iColumn;
            }
        } else if (bInProblem) {
            vectorProblems.push_back({iFirstColumn, iColumn - 1});
            bInProblem = false;
        }
    }

    if (bInProblem) {
        vectorProblems.push_back({iFirstColumn, m_iColumnCount - 1});
    }

    return vectorProblems;
}

char Day06::ProblemOperation(const ProblemColumns& problemcolumns) const {
    const auto& stringInputRow = m_vectorWorksheet[m_iRowCount - 1];
    for (int iColumn = problemcolumns.m_iFirstColumn; iColumn <= problemcolumns.m_iLastColumn;
         ++iColumn) {
        if (stringInputRow[iColumn] == '+' || stringInputRow[iColumn] == '*') {
            return stringInputRow[iColumn];
        }
    }
    throw std::invalid_argument("Missing worksheet operator");
}

// -----------------------------------------------------------------------------
// Extractors
// -----------------------------------------------------------------------------

std::vector<std::int64_t> Day06::ReadNumbersByRow(const ProblemColumns& problemcolumns) const {
    std::vector<std::int64_t> vectorNumbers;
    vectorNumbers.reserve(m_iRowCount);

    for (int iRow = 0; iRow < m_iRowCount - 1; ++iRow) {
        std::string stringNumber = m_vectorWorksheet[iRow].substr(
            problemcolumns.m_iFirstColumn,
            problemcolumns.m_iLastColumn - problemcolumns.m_iFirstColumn + 1);
        stringNumber.erase(0, stringNumber.find_first_not_of(' '));
        stringNumber.erase(stringNumber.find_last_not_of(' ') + 1);
        vectorNumbers.push_back(core::ParseInteger<std::int64_t>(stringNumber));
    }
    return vectorNumbers;
}

std::vector<std::int64_t> Day06::ReadNumbersByColumn(const ProblemColumns& problemcolumns) const {
    std::vector<std::int64_t> vectorNumbers;
    vectorNumbers.reserve(problemcolumns.m_iLastColumn - problemcolumns.m_iFirstColumn + 1);

    for (int iColumn = problemcolumns.m_iFirstColumn; iColumn <= problemcolumns.m_iLastColumn;
         ++iColumn) {
        std::string stringNumber;
        for (int iRow = 0; iRow < m_iRowCount - 1; ++iRow) {
            char iDigit = m_vectorWorksheet[iRow][iColumn];
            if (iDigit != ' ')
                stringNumber.push_back(iDigit);
        }
        vectorNumbers.push_back(core::ParseInteger<std::int64_t>(stringNumber));
    }
    return vectorNumbers;
}

// -----------------------------------------------------------------------------
// Evaluation
// -----------------------------------------------------------------------------

std::int64_t Day06::EvaluateProblem(std::span<const std::int64_t> spanNumbers, char iOperation) {
    if (iOperation == '+') {
        std::int64_t iSum = 0;
        for (auto iNumber : spanNumbers)
            iSum += iNumber;
        return iSum;
    }

    std::int64_t iProduct = 1;
    for (auto iNumber : spanNumbers)
        iProduct *= iNumber;
    return iProduct;
}

template <typename NUMBER_READER>
std::int64_t Day06::GrandTotal(NUMBER_READER&& numberreader) const {
    std::int64_t iTotal = 0;

    for (const auto& problemcolumns : FindProblems()) {
        auto vectorNumbers = numberreader(problemcolumns);
        char iOperation = ProblemOperation(problemcolumns);
        iTotal += EvaluateProblem(vectorNumbers, iOperation);
    }
    return iTotal;
}

// -----------------------------------------------------------------------------
// Parts
// -----------------------------------------------------------------------------

std::string Day06::Part1() {
    return std::to_string(GrandTotal(
        [this](const ProblemColumns& problemcolumns) { return ReadNumbersByRow(problemcolumns); }));
}

std::string Day06::Part2() {
    return std::to_string(GrandTotal([this](const ProblemColumns& problemcolumns) {
        return ReadNumbersByColumn(problemcolumns);
    }));
}
