#include "days/day02.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <array>
#include <limits>
#include <stdexcept>

// ------------------------------------------------------------
// Registration
// ------------------------------------------------------------
namespace {
const core::DayRegistration<Day02> dayregistration{2};
} // namespace

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

static constexpr std::array<std::uint64_t, 20> MakePowersOfTen() {
    std::array<std::uint64_t, 20> arrayPowers{};
    std::uint64_t uPowerOfTen = 1;
    for (std::size_t uExponent = 0; uExponent < arrayPowers.size(); ++uExponent) {
        arrayPowers[uExponent] = uPowerOfTen;
        if (uExponent + 1 < arrayPowers.size())
            uPowerOfTen *= 10;
    }
    return arrayPowers;
}

static constexpr auto arrayPowersOfTen = MakePowersOfTen();

int Day02::ShortestRepeatingBlockLength(const std::string& stringDigits) {
    const int iDigitCount = static_cast<int>(stringDigits.size());
    for (int iBlockLength = 1; iBlockLength <= iDigitCount / 2; ++iBlockLength) {
        if (iDigitCount % iBlockLength != 0)
            continue;

        const std::string_view stringDigitBlock{stringDigits.data(),
                                                static_cast<std::size_t>(iBlockLength)};
        bool bRepeats = true;

        for (int iBlockOffset = iBlockLength; iBlockOffset < iDigitCount;
             iBlockOffset += iBlockLength) {
            if (std::string_view{stringDigits.data() + iBlockOffset,
                                 static_cast<std::size_t>(iBlockLength)} != stringDigitBlock) {
                bRepeats = false;
                break;
            }
        }

        if (bRepeats)
            return iBlockLength;
    }
    return iDigitCount;
}

// ------------------------------------------------------------
// Input
// ------------------------------------------------------------

void Day02::SetInput(const std::vector<std::string>& vectorInputLines) {
    m_vectorProductIdRanges.clear();
    if (vectorInputLines.empty())
        return;

    const auto stringLine = core::Trim(vectorInputLines.front());
    if (!stringLine.empty() && stringLine.back() == ',')
        throw std::invalid_argument("Trailing comma in ID ranges");
    std::size_t uRangeOffset = 0;

    while (uRangeOffset < stringLine.size()) {
        std::size_t uCommaOffset = stringLine.find(',', uRangeOffset);
        if (uCommaOffset == std::string::npos)
            uCommaOffset = stringLine.size();

        std::string_view stringIdRange(stringLine.data() + uRangeOffset,
                                       uCommaOffset - uRangeOffset);
        std::size_t uDashOffset = stringIdRange.find('-');

        if (uDashOffset == std::string_view::npos)
            throw std::invalid_argument("Expected an ID range");
        const auto iFirstId =
            core::ParseInteger<std::int64_t>(stringIdRange.substr(0, uDashOffset));
        const auto iLastId =
            core::ParseInteger<std::int64_t>(stringIdRange.substr(uDashOffset + 1));
        if (iFirstId < 0 || iLastId < iFirstId)
            throw std::invalid_argument("Invalid ID range");

        m_vectorProductIdRanges.emplace_back(iFirstId, iLastId);
        uRangeOffset = uCommaOffset + 1;
    }
}

// ------------------------------------------------------------
// Part 1
// ------------------------------------------------------------

std::string Day02::Part1() {
    std::int64_t iInvalidIdSum = 0;

    for (auto [iFirstId, iLastId] : m_vectorProductIdRanges) {
        int iMaxDigitCount = static_cast<int>(std::to_string(iLastId).size());

        for (int iBlockLength = 1; 2 * iBlockLength <= iMaxDigitCount; ++iBlockLength) {
            std::int64_t iBlockBase = arrayPowersOfTen[iBlockLength];
            std::int64_t iRepetitionFactor = iBlockBase + 1;

            std::int64_t iSmallestBlock = arrayPowersOfTen[iBlockLength - 1];
            std::int64_t iLargestBlock = iBlockBase - 1;

            std::int64_t iFirstBlock =
                iFirstId / iRepetitionFactor + (iFirstId % iRepetitionFactor != 0);
            std::int64_t iLastBlock = iLastId / iRepetitionFactor;

            iFirstBlock = std::max(iFirstBlock, iSmallestBlock);
            iLastBlock = std::min(iLastBlock, iLargestBlock);
            if (iFirstBlock > iLastBlock)
                continue;

            // Sum the arithmetic progression without enumerating every repeated ID.
            auto iBlockCount = iLastBlock - iFirstBlock + 1;
            auto iEndpointSum = iFirstBlock + iLastBlock;
            if (iBlockCount % 2 == 0)
                iBlockCount /= 2;
            else
                iEndpointSum /= 2;
            const auto iRemainingSumCapacity =
                std::numeric_limits<std::int64_t>::max() - iInvalidIdSum;
            if (iEndpointSum > iRemainingSumCapacity / iRepetitionFactor / iBlockCount)
                throw std::overflow_error("ID sum exceeds int64_t");
            iInvalidIdSum += iEndpointSum * iBlockCount * iRepetitionFactor;
        }
    }

    return std::to_string(iInvalidIdSum);
}

// ------------------------------------------------------------
// Part 2
// ------------------------------------------------------------

std::string Day02::Part2() {
    std::int64_t iInvalidIdSum = 0;

    for (auto [iFirstId, iLastId] : m_vectorProductIdRanges) {
        int iMaxDigitCount = static_cast<int>(std::to_string(iLastId).size());

        for (int iIdDigitCount = 2; iIdDigitCount <= iMaxDigitCount; ++iIdDigitCount) {
            const auto uIdDigitBase = arrayPowersOfTen[iIdDigitCount];

            for (int iRepetitionCount = 2; iRepetitionCount <= iIdDigitCount; ++iRepetitionCount) {
                if (iIdDigitCount % iRepetitionCount != 0)
                    continue;

                int iBlockLength = iIdDigitCount / iRepetitionCount;
                std::int64_t iBlockBase = arrayPowersOfTen[iBlockLength];
                const auto iRepetitionFactor =
                    static_cast<std::int64_t>((uIdDigitBase - 1) / (iBlockBase - 1));

                std::int64_t iSmallestBlock = arrayPowersOfTen[iBlockLength - 1];
                std::int64_t iLargestBlock = iBlockBase - 1;

                std::int64_t iFirstBlock =
                    iFirstId / iRepetitionFactor + (iFirstId % iRepetitionFactor != 0);
                std::int64_t iLastBlock = iLastId / iRepetitionFactor;

                iFirstBlock = std::max(iFirstBlock, iSmallestBlock);
                iLastBlock = std::min(iLastBlock, iLargestBlock);
                if (iFirstBlock > iLastBlock)
                    continue;

                for (std::int64_t iBlock = iFirstBlock; iBlock <= iLastBlock; ++iBlock) {
                    std::string stringBlock = std::to_string(iBlock);
                    if (ShortestRepeatingBlockLength(stringBlock) !=
                        static_cast<int>(stringBlock.size()))
                        continue;
                    const auto iRepeatedId = iBlock * iRepetitionFactor;
                    if (iRepeatedId > std::numeric_limits<std::int64_t>::max() - iInvalidIdSum)
                        throw std::overflow_error("ID sum exceeds int64_t");
                    iInvalidIdSum += iRepeatedId;
                }
            }
        }
    }

    return std::to_string(iInvalidIdSum);
}
