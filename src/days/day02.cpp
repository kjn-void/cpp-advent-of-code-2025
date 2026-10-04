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
const core::Drg<Day02> drgDay{2};
} // namespace

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

static constexpr std::array<std::uint64_t, 20> RgcoefBuildPowersOfTen() {
    std::array<std::uint64_t, 20> rgcoefPowers{};
    std::uint64_t coefPowerOfTen = 1;
    for (std::size_t icoefExponent = 0; icoefExponent < rgcoefPowers.size(); ++icoefExponent) {
        rgcoefPowers[icoefExponent] = coefPowerOfTen;
        if (icoefExponent + 1 < rgcoefPowers.size())
            coefPowerOfTen *= 10;
    }
    return rgcoefPowers;
}

static constexpr auto rgcoefPowersOfTen = RgcoefBuildPowersOfTen();

int Day02::LenFindSmallestBlock(const std::string& txtDigits) {
    const int cdig = static_cast<int>(txtDigits.size());
    for (int lenBlock = 1; lenBlock <= cdig / 2; ++lenBlock) {
        if (cdig % lenBlock != 0)
            continue;

        const std::string_view txtBlockFirst{txtDigits.data(), static_cast<std::size_t>(lenBlock)};
        bool fRepeated = true;

        for (int offBlock = lenBlock; offBlock < cdig; offBlock += lenBlock) {
            if (std::string_view{txtDigits.data() + offBlock, static_cast<std::size_t>(lenBlock)} !=
                txtBlockFirst) {
                fRepeated = false;
                break;
            }
        }

        if (fRepeated)
            return lenBlock;
    }
    return cdig;
}

// ------------------------------------------------------------
// Input
// ------------------------------------------------------------

void Day02::SetInput(const std::vector<std::string>& rgusLines) {
    rgrngIds_.clear();
    if (rgusLines.empty())
        return;

    const auto usLine = core::UsTrim(rgusLines.front());
    if (!usLine.empty() && usLine.back() == ',')
        throw std::invalid_argument("Trailing comma in ID ranges");
    std::size_t offRange = 0;

    while (offRange < usLine.size()) {
        std::size_t offComma = usLine.find(',', offRange);
        if (offComma == std::string::npos)
            offComma = usLine.size();

        std::string_view usRange(usLine.data() + offRange, offComma - offRange);
        std::size_t offDash = usRange.find('-');

        if (offDash == std::string_view::npos)
            throw std::invalid_argument("Expected an ID range");
        const auto idFirst = core::ValParseInteger<std::int64_t>(usRange.substr(0, offDash));
        const auto idLast = core::ValParseInteger<std::int64_t>(usRange.substr(offDash + 1));
        if (idFirst < 0 || idLast < idFirst)
            throw std::invalid_argument("Invalid ID range");

        rgrngIds_.emplace_back(idFirst, idLast);
        offRange = offComma + 1;
    }
}

// ------------------------------------------------------------
// Part 1
// ------------------------------------------------------------

std::string Day02::TxtPart1() {
    std::int64_t valSumInvalidIds = 0;

    for (auto [idFirst, idLast] : rgrngIds_) {
        int cdigLast = static_cast<int>(std::to_string(idLast).size());

        for (int lenBlock = 1; 2 * lenBlock <= cdigLast; ++lenBlock) {
            std::int64_t coefBase = rgcoefPowersOfTen[lenBlock];
            std::int64_t coefRepeat = coefBase + 1;

            std::int64_t blkFirstOfLength = rgcoefPowersOfTen[lenBlock - 1];
            std::int64_t blkLastOfLength = coefBase - 1;

            std::int64_t blkFirst = idFirst / coefRepeat + (idFirst % coefRepeat != 0);
            std::int64_t blkLast = idLast / coefRepeat;

            blkFirst = std::max(blkFirst, blkFirstOfLength);
            blkLast = std::min(blkLast, blkLastOfLength);
            if (blkFirst > blkLast)
                continue;

            // Sum the arithmetic progression without enumerating every repeated ID.
            auto cblk = blkLast - blkFirst + 1;
            auto valSumEndpointBlocks = blkFirst + blkLast;
            if (cblk % 2 == 0)
                cblk /= 2;
            else
                valSumEndpointBlocks /= 2;
            const auto valSumRemaining =
                std::numeric_limits<std::int64_t>::max() - valSumInvalidIds;
            if (valSumEndpointBlocks > valSumRemaining / coefRepeat / cblk)
                throw std::overflow_error("ID sum exceeds int64_t");
            valSumInvalidIds += valSumEndpointBlocks * cblk * coefRepeat;
        }
    }

    return std::to_string(valSumInvalidIds);
}

// ------------------------------------------------------------
// Part 2
// ------------------------------------------------------------

std::string Day02::TxtPart2() {
    std::int64_t valSumInvalidIds = 0;

    for (auto [idFirst, idLast] : rgrngIds_) {
        int cdigLast = static_cast<int>(std::to_string(idLast).size());

        for (int cdigId = 2; cdigId <= cdigLast; ++cdigId) {
            const auto coefIdBase = rgcoefPowersOfTen[cdigId];

            for (int cblkPerId = 2; cblkPerId <= cdigId; ++cblkPerId) {
                if (cdigId % cblkPerId != 0)
                    continue;

                int lenBlock = cdigId / cblkPerId;
                std::int64_t coefBlockBase = rgcoefPowersOfTen[lenBlock];
                const auto coefRepeat =
                    static_cast<std::int64_t>((coefIdBase - 1) / (coefBlockBase - 1));

                std::int64_t blkFirstOfLength = rgcoefPowersOfTen[lenBlock - 1];
                std::int64_t blkLastOfLength = coefBlockBase - 1;

                std::int64_t blkFirst = idFirst / coefRepeat + (idFirst % coefRepeat != 0);
                std::int64_t blkLast = idLast / coefRepeat;

                blkFirst = std::max(blkFirst, blkFirstOfLength);
                blkLast = std::min(blkLast, blkLastOfLength);
                if (blkFirst > blkLast)
                    continue;

                for (std::int64_t blk = blkFirst; blk <= blkLast; ++blk) {
                    std::string txtBlock = std::to_string(blk);
                    if (LenFindSmallestBlock(txtBlock) != static_cast<int>(txtBlock.size()))
                        continue;
                    const auto idRepeated = blk * coefRepeat;
                    if (idRepeated > std::numeric_limits<std::int64_t>::max() - valSumInvalidIds)
                        throw std::overflow_error("ID sum exceeds int64_t");
                    valSumInvalidIds += idRepeated;
                }
            }
        }
    }

    return std::to_string(valSumInvalidIds);
}
