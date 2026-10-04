#include "days/day10.h"
#include "core/Parallel.h"
#include "core/Register.h"

#include "core/Parse.h"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

// Registration
namespace {
const core::Drg<Day10> drgDay{10};
} // namespace

// ------------------------------------------------------------
// Parsing helpers
// ------------------------------------------------------------

std::vector<int> Day10::RgvalParseList(std::string_view usList) {
    std::vector<int> rgvalParsed;
    if (usList.size() < 2)
        return rgvalParsed;

    const auto usContents = usList.substr(1, usList.size() - 2);
    std::string usToken;
    std::istringstream inList(std::string{usContents});

    while (std::getline(inList, usToken, ',')) {
        rgvalParsed.push_back(core::ValParseInteger<int>(usToken));
    }
    if (!usContents.empty() && usContents.back() == ',')
        throw std::invalid_argument("Trailing comma in machine list");
    return rgvalParsed;
}

void Day10::SetInput(const std::vector<std::string>& rgusLines) {
    rgmch_.clear();

    for (const auto& usLine : rgusLines) {
        if (usLine.empty())
            continue;

        // lights
        auto offLightsBegin = usLine.find('[');
        auto offLightsEnd = usLine.find(']');
        if (offLightsBegin == std::string::npos || offLightsEnd == std::string::npos ||
            offLightsEnd <= offLightsBegin)
            throw std::invalid_argument("Missing machine lights");

        std::vector<int> rgfDiagram;
        for (char chLight : usLine.substr(offLightsBegin + 1, offLightsEnd - offLightsBegin - 1)) {
            if (chLight != '#' && chLight != '.')
                throw std::invalid_argument("Invalid light state");
            rgfDiagram.push_back(chLight == '#' ? 1 : 0);
        }

        // joltage
        std::vector<int> jvRequired;
        auto offJoltageBegin = usLine.find('{');
        auto offJoltageEnd = usLine.find('}');
        if (offJoltageBegin == std::string::npos || offJoltageEnd == std::string::npos ||
            offJoltageBegin <= offLightsEnd || offJoltageEnd <= offJoltageBegin)
            throw std::invalid_argument("Missing machine joltage");
        jvRequired =
            RgvalParseList(usLine.substr(offJoltageBegin, offJoltageEnd - offJoltageBegin + 1));

        // buttons
        std::vector<std::vector<int>> rgbtn;
        auto usButtons = core::UsTrim(
            std::string_view(usLine).substr(offLightsEnd + 1, offJoltageBegin - offLightsEnd - 1));

        std::size_t offButtonBegin = 0;
        while (!usButtons.empty()) {
            if (usButtons.front() != '(')
                throw std::invalid_argument("Expected a machine button");
            auto offButtonEnd = usButtons.find(')', offButtonBegin);
            if (offButtonEnd == std::string::npos)
                throw std::invalid_argument("Unclosed machine button");
            rgbtn.push_back(RgvalParseList(
                usButtons.substr(offButtonBegin, offButtonEnd - offButtonBegin + 1)));
            usButtons = core::UsTrim(usButtons.substr(offButtonEnd + 1));
        }

        if (!core::UsTrim(std::string_view(usLine).substr(0, offLightsBegin)).empty() ||
            !core::UsTrim(std::string_view(usLine).substr(offJoltageEnd + 1)).empty())
            throw std::invalid_argument("Unexpected text around machine");
        if (rgfDiagram.empty() || rgfDiagram.size() != jvRequired.size() ||
            std::ranges::any_of(jvRequired, [](int jolRequired) { return jolRequired < 0; }))
            throw std::invalid_argument("Invalid machine targets");
        for (auto& btn : rgbtn) {
            std::ranges::sort(btn);
            if (std::ranges::any_of(btn,
                                    [&](int ictr) {
                                        return ictr < 0 ||
                                               ictr >= static_cast<int>(rgfDiagram.size());
                                    }) ||
                std::adjacent_find(btn.begin(), btn.end()) != btn.end())
                throw std::invalid_argument("Invalid machine button index");
        }
        // Repeated or empty buttons cannot improve a minimum-press solution.
        std::erase_if(rgbtn, [](const auto& btn) { return btn.empty(); });
        std::ranges::sort(rgbtn);
        rgbtn.erase(std::unique(rgbtn.begin(), rgbtn.end()), rgbtn.end());
        rgmch_.push_back({std::move(rgfDiagram), std::move(jvRequired), std::move(rgbtn)});
    }
}

// ------------------------------------------------------------
// Part 1 — GF(2) Gaussian elimination
// ------------------------------------------------------------

int Day10::CprSolveLights(const Mch& mch) {
    int cfLights = static_cast<int>(mch.rgfDiagram.size());
    int cbtn = static_cast<int>(mch.rgbtn.size());

    std::vector<std::vector<int>> matLights(cfLights, std::vector<int>(cbtn + 1, 0));
    for (int rwLight = 0; rwLight < cfLights; ++rwLight)
        matLights[rwLight][cbtn] = mch.rgfDiagram[rwLight];

    for (int colButton = 0; colButton < cbtn; ++colButton)
        for (int rwLight : mch.rgbtn[colButton])
            if (rwLight < cfLights)
                matLights[rwLight][colButton] = 1;

    int rwPivot = 0;
    std::vector<int> mpcolrwPivot(cbtn, -1);

    for (int colPivot = 0; colPivot < cbtn && rwPivot < cfLights; ++colPivot) {
        int rwSelected = -1;
        for (int rw = rwPivot; rw < cfLights; ++rw) {
            if (matLights[rw][colPivot]) {
                rwSelected = rw;
                break;
            }
        }
        if (rwSelected == -1)
            continue;

        std::swap(matLights[rwPivot], matLights[rwSelected]);
        mpcolrwPivot[colPivot] = rwPivot;

        for (int rw = 0; rw < cfLights; ++rw) {
            if (rw != rwPivot && matLights[rw][colPivot]) {
                for (int colCoefficient = colPivot; colCoefficient <= cbtn; ++colCoefficient)
                    matLights[rw][colCoefficient] ^= matLights[rwPivot][colCoefficient];
            }
        }
        rwPivot++;
    }

    for (int rw = rwPivot; rw < cfLights; ++rw) {
        if (matLights[rw][cbtn] != 0)
            throw std::runtime_error("Unreachable light target");
    }

    std::vector<int> rgcolFree;
    for (int colButton = 0; colButton < cbtn; ++colButton)
        if (mpcolrwPivot[colButton] == -1)
            rgcolFree.push_back(colButton);

    int cprBest = cbtn + 1;
    std::vector<int> mpibtnbitParity(cbtn, 0);
    const auto fnSearch = [&](auto&& fnRecurSearch, std::size_t icolFree, int cpr) -> void {
        if (cpr >= cprBest)
            return;
        if (icolFree < rgcolFree.size()) {
            const int colFree = rgcolFree[icolFree];
            mpibtnbitParity[colFree] = 0;
            fnRecurSearch(fnRecurSearch, icolFree + 1, cpr);
            mpibtnbitParity[colFree] = 1;
            fnRecurSearch(fnRecurSearch, icolFree + 1, cpr + 1);
            return;
        }
        for (int colButton = cbtn - 1; colButton >= 0; --colButton) {
            if (mpcolrwPivot[colButton] == -1)
                continue;
            const int rw = mpcolrwPivot[colButton];
            int bitPress = matLights[rw][cbtn];
            for (int colCoefficient = colButton + 1; colCoefficient < cbtn; ++colCoefficient)
                bitPress ^= matLights[rw][colCoefficient] & mpibtnbitParity[colCoefficient];
            mpibtnbitParity[colButton] = bitPress;
            cpr += bitPress;
        }
        cprBest = std::min(cprBest, cpr);
    };
    fnSearch(fnSearch, 0, 0);

    return cprBest;
}

// ------------------------------------------------------------
// Part 2 — exact integer recursion on the binary digits of press counts
// ------------------------------------------------------------

std::int64_t Day10::CprSolveJoltage(const Mch& mch) {
    struct Hashrgval {
        std::size_t operator()(const std::vector<int>& rgval) const noexcept {
            std::size_t hash = 0;
            for (int valComponent : rgval)
                hash ^= std::hash<int>{}(valComponent) + 0x9e3779b9U + (hash << 6) + (hash >> 2);
            return hash;
        }
    };
    struct Chc {
        std::vector<int> jvIncrement;
        int cpr;
    };

    const auto cictr = mch.jvRequired.size();
    std::unordered_map<std::vector<int>, std::vector<Chc>, Hashrgval> mpparrgchc;
    std::vector<int> jvIncrement(cictr, 0);
    const auto fnEnumerateParity = [&](auto&& fnRecurSolve, std::size_t ibtn, int cpr) -> void {
        if (ibtn == mch.rgbtn.size()) {
            auto parTarget = jvIncrement;
            for (auto& valComponent : parTarget)
                valComponent %= 2;
            mpparrgchc[parTarget].push_back({jvIncrement, cpr});
            return;
        }
        fnRecurSolve(fnRecurSolve, ibtn + 1, cpr);
        for (int ictr : mch.rgbtn[ibtn])
            ++jvIncrement[ictr];
        fnRecurSolve(fnRecurSolve, ibtn + 1, cpr + 1);
        for (int ictr : mch.rgbtn[ibtn])
            --jvIncrement[ictr];
    };
    fnEnumerateParity(fnEnumerateParity, 0, 0);

    using Optcpr = std::optional<std::int64_t>;
    std::unordered_map<std::vector<int>, Optcpr, Hashrgval> mpjvoptcprMemo;
    const auto fnFewestPresses = [&](auto&& fnRecurSolve,
                                     const std::vector<int>& jvRemaining) -> Optcpr {
        if (std::ranges::all_of(jvRemaining, [](int jolRemaining) { return jolRemaining == 0; }))
            return 0;
        if (const auto itCachedPressCount = mpjvoptcprMemo.find(jvRemaining);
            itCachedPressCount != mpjvoptcprMemo.end())
            return itCachedPressCount->second;

        auto parTarget = jvRemaining;
        for (auto& valComponent : parTarget)
            valComponent %= 2;
        const auto itChoices = mpparrgchc.find(parTarget);
        Optcpr optcprBest;
        if (itChoices != mpparrgchc.end()) {
            for (const auto& chc : itChoices->second) {
                std::vector<int> jvHalf(cictr);
                bool fFeasible = true;
                for (std::size_t ictr = 0; ictr < cictr; ++ictr) {
                    if (chc.jvIncrement[ictr] > jvRemaining[ictr]) {
                        fFeasible = false;
                        break;
                    }
                    jvHalf[ictr] = (jvRemaining[ictr] - chc.jvIncrement[ictr]) / 2;
                }
                if (!fFeasible)
                    continue;
                // Each press adds at most one to any counter.
                const auto cprLowerBound = chc.cpr + 2 * std::int64_t{std::ranges::max(jvHalf)};
                if (optcprBest && cprLowerBound >= *optcprBest)
                    continue;
                if (const auto optcprRemaining = fnRecurSolve(fnRecurSolve, jvHalf)) {
                    const auto cprTotal = chc.cpr + 2 * *optcprRemaining;
                    if (!optcprBest || cprTotal < *optcprBest)
                        optcprBest = cprTotal;
                }
            }
        }
        mpjvoptcprMemo.emplace(jvRemaining, optcprBest);
        return optcprBest;
    };

    // Any press vector is uniquely x = odd + 2 * rest. Matching target parity
    // makes (target - A * odd) / 2 an exact, smaller integer subproblem.
    const auto optcprMinimum = fnFewestPresses(fnFewestPresses, mch.jvRequired);
    if (!optcprMinimum)
        throw std::runtime_error("Unreachable joltage target");
    return *optcprMinimum;
}

// ------------------------------------------------------------
// Day interface
// ------------------------------------------------------------

std::string Day10::TxtPart1() {
    const std::int64_t cprTotal =
        core::ValSumIndexed(rgmch_.size(), [&](std::size_t imch) -> std::int64_t {
            const auto& mch = rgmch_[imch];
            if (mch.rgfDiagram.empty())
                return 0;
            return static_cast<std::int64_t>(CprSolveLights(mch));
        });

    return std::to_string(cprTotal);
}

std::string Day10::TxtPart2() {
    const std::int64_t cprTotal =
        core::ValSumIndexed(rgmch_.size(), [&](std::size_t imch) -> std::int64_t {
            const auto& mch = rgmch_[imch];
            if (mch.jvRequired.empty())
                return 0;
            return static_cast<std::int64_t>(CprSolveJoltage(mch));
        });

    return std::to_string(cprTotal);
}
