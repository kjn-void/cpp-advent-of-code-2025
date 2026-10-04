#include "days/day06.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <stdexcept>

// -----------------------------------------------------------------------------
// Registration
// -----------------------------------------------------------------------------
namespace {
const core::Drg<Day06> drgDay{6};
} // namespace

// -----------------------------------------------------------------------------
// Input
// -----------------------------------------------------------------------------

void Day06::SetInput(const std::vector<std::string>& rgusLines) {
    gridWorksheet_ = rgusLines;

    // normalize width
    ccol_ = 0;
    for (const auto& usRow : gridWorksheet_) {
        ccol_ = std::max(ccol_, static_cast<int>(usRow.size()));
    }
    for (auto& usRow : gridWorksheet_) {
        if (static_cast<int>(usRow.size()) < ccol_) {
            usRow.append(ccol_ - usRow.size(), ' ');
        }
    }

    crw_ = static_cast<int>(gridWorksheet_.size());
}

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

std::vector<Day06::Prb> Day06::RgprbFindProblems() const {
    std::vector<bool> mpcolfBlank(ccol_, true);

    for (int col = 0; col < ccol_; ++col) {
        for (int rw = 0; rw < crw_; ++rw) {
            if (gridWorksheet_[rw][col] != ' ') {
                mpcolfBlank[col] = false;
                break;
            }
        }
    }

    std::vector<Prb> rgprb;
    bool fInProblem = false;
    int colFirst = 0;

    for (int col = 0; col < ccol_; ++col) {
        if (!mpcolfBlank[col]) {
            if (!fInProblem) {
                fInProblem = true;
                colFirst = col;
            }
        } else if (fInProblem) {
            rgprb.push_back({colFirst, col - 1});
            fInProblem = false;
        }
    }

    if (fInProblem) {
        rgprb.push_back({colFirst, ccol_ - 1});
    }

    return rgprb;
}

char Day06::ChProblemOperator(const Prb& prb) const {
    const auto& usRow = gridWorksheet_[crw_ - 1];
    for (int col = prb.colFirst; col <= prb.colLast; ++col) {
        if (usRow[col] == '+' || usRow[col] == '*') {
            return usRow[col];
        }
    }
    throw std::invalid_argument("Missing worksheet operator");
}

// -----------------------------------------------------------------------------
// Extractors
// -----------------------------------------------------------------------------

std::vector<std::int64_t> Day06::RgvalReadRows(const Prb& prb) const {
    std::vector<std::int64_t> rgvalOperands;
    rgvalOperands.reserve(crw_);

    for (int rw = 0; rw < crw_ - 1; ++rw) {
        std::string usNumber =
            gridWorksheet_[rw].substr(prb.colFirst, prb.colLast - prb.colFirst + 1);
        usNumber.erase(0, usNumber.find_first_not_of(' '));
        usNumber.erase(usNumber.find_last_not_of(' ') + 1);
        rgvalOperands.push_back(core::ValParseInteger<std::int64_t>(usNumber));
    }
    return rgvalOperands;
}

std::vector<std::int64_t> Day06::RgvalReadColumns(const Prb& prb) const {
    std::vector<std::int64_t> rgvalOperands;
    rgvalOperands.reserve(prb.colLast - prb.colFirst + 1);

    for (int col = prb.colFirst; col <= prb.colLast; ++col) {
        std::string usNumber;
        for (int rw = 0; rw < crw_ - 1; ++rw) {
            char chDigit = gridWorksheet_[rw][col];
            if (chDigit != ' ')
                usNumber.push_back(chDigit);
        }
        rgvalOperands.push_back(core::ValParseInteger<std::int64_t>(usNumber));
    }
    return rgvalOperands;
}

// -----------------------------------------------------------------------------
// Evaluation
// -----------------------------------------------------------------------------

std::int64_t Day06::ValEvaluateProblem(std::span<const std::int64_t> rgvalOperands,
                                       char chOperator) {
    if (chOperator == '+') {
        std::int64_t valSumOperands = 0;
        for (auto valOperand : rgvalOperands)
            valSumOperands += valOperand;
        return valSumOperands;
    }

    std::int64_t valProductOperands = 1;
    for (auto valOperand : rgvalOperands)
        valProductOperands *= valOperand;
    return valProductOperands;
}

template <typename Fn> std::int64_t Day06::ValGrandTotal(Fn&& fnReadNumbers) const {
    std::int64_t valGrandTotal = 0;

    for (const auto& prb : RgprbFindProblems()) {
        auto rgvalOperands = fnReadNumbers(prb);
        char chOperator = ChProblemOperator(prb);
        valGrandTotal += ValEvaluateProblem(rgvalOperands, chOperator);
    }
    return valGrandTotal;
}

// -----------------------------------------------------------------------------
// Parts
// -----------------------------------------------------------------------------

std::string Day06::TxtPart1() {
    return std::to_string(ValGrandTotal([this](const Prb& prb) { return RgvalReadRows(prb); }));
}

std::string Day06::TxtPart2() {
    return std::to_string(ValGrandTotal([this](const Prb& prb) { return RgvalReadColumns(prb); }));
}
