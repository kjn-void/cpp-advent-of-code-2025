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

std::vector<Day06::Blk> Day06::RgblkFind() const {
    std::vector<bool> mpcolfBlank(ccol_, true);

    for (int col = 0; col < ccol_; ++col) {
        for (int rw = 0; rw < crw_; ++rw) {
            if (gridWorksheet_[rw][col] != ' ') {
                mpcolfBlank[col] = false;
                break;
            }
        }
    }

    std::vector<Blk> rgblk;
    bool fInBlock = false;
    int colFirst = 0;

    for (int col = 0; col < ccol_; ++col) {
        if (!mpcolfBlank[col]) {
            if (!fInBlock) {
                fInBlock = true;
                colFirst = col;
            }
        } else if (fInBlock) {
            rgblk.push_back({colFirst, col - 1});
            fInBlock = false;
        }
    }

    if (fInBlock) {
        rgblk.push_back({colFirst, ccol_ - 1});
    }

    return rgblk;
}

char Day06::ChGetOperator(const Blk& blk) const {
    const auto& usRow = gridWorksheet_[crw_ - 1];
    for (int col = blk.colFirst; col <= blk.colLast; ++col) {
        if (usRow[col] == '+' || usRow[col] == '*') {
            return usRow[col];
        }
    }
    throw std::invalid_argument("Missing worksheet operator");
}

// -----------------------------------------------------------------------------
// Extractors
// -----------------------------------------------------------------------------

std::vector<std::int64_t> Day06::RgvalExtractPart1(const Blk& blk) const {
    std::vector<std::int64_t> rgvalOperands;
    rgvalOperands.reserve(crw_);

    for (int rw = 0; rw < crw_ - 1; ++rw) {
        std::string usNumber =
            gridWorksheet_[rw].substr(blk.colFirst, blk.colLast - blk.colFirst + 1);
        usNumber.erase(0, usNumber.find_first_not_of(' '));
        usNumber.erase(usNumber.find_last_not_of(' ') + 1);
        rgvalOperands.push_back(core::ValParseInteger<std::int64_t>(usNumber));
    }
    return rgvalOperands;
}

std::vector<std::int64_t> Day06::RgvalExtractPart2(const Blk& blk) const {
    std::vector<std::int64_t> rgvalOperands;
    rgvalOperands.reserve(blk.colLast - blk.colFirst + 1);

    for (int col = blk.colFirst; col <= blk.colLast; ++col) {
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

std::int64_t Day06::ValEvaluateOperands(std::span<const std::int64_t> rgvalOperands,
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

template <typename Fn> std::int64_t Day06::ValEvaluateBlocks(Fn&& fnExtractOperands) const {
    std::int64_t valSumProblems = 0;

    for (const auto& blk : RgblkFind()) {
        auto rgvalOperands = fnExtractOperands(blk);
        char chOperator = ChGetOperator(blk);
        valSumProblems += ValEvaluateOperands(rgvalOperands, chOperator);
    }
    return valSumProblems;
}

// -----------------------------------------------------------------------------
// Parts
// -----------------------------------------------------------------------------

std::string Day06::TxtPart1() {
    return std::to_string(
        ValEvaluateBlocks([this](const Blk& blk) { return RgvalExtractPart1(blk); }));
}

std::string Day06::TxtPart2() {
    return std::to_string(
        ValEvaluateBlocks([this](const Blk& blk) { return RgvalExtractPart2(blk); }));
}
