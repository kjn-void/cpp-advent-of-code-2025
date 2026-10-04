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
const core::DayRegistration<Day10> dayregistration{10};
} // namespace

// ------------------------------------------------------------
// Parsing helpers
// ------------------------------------------------------------

std::vector<int> Day10::ParseIntegerList(std::string_view stringList) {
    std::vector<int> vectorIntegers;
    if (stringList.size() < 2)
        return vectorIntegers;

    const auto stringListContents = stringList.substr(1, stringList.size() - 2);
    std::string stringInteger;
    std::istringstream istringstreamListInput(std::string{stringListContents});

    while (std::getline(istringstreamListInput, stringInteger, ',')) {
        vectorIntegers.push_back(core::ParseInteger<int>(stringInteger));
    }
    if (!stringListContents.empty() && stringListContents.back() == ',')
        throw std::invalid_argument("Trailing comma in machine list");
    return vectorIntegers;
}

void Day10::SetInput(const std::vector<std::string>& vectorInputLines) {
    m_vectorMachines.clear();

    for (const auto& stringLine : vectorInputLines) {
        if (stringLine.empty())
            continue;

        // lights
        auto uLightsOpen = stringLine.find('[');
        auto uLightsClose = stringLine.find(']');
        if (uLightsOpen == std::string::npos || uLightsClose == std::string::npos ||
            uLightsClose <= uLightsOpen)
            throw std::invalid_argument("Missing machine lights");

        std::vector<int> vectorLightDiagram;
        for (char iLightSymbol :
             stringLine.substr(uLightsOpen + 1, uLightsClose - uLightsOpen - 1)) {
            if (iLightSymbol != '#' && iLightSymbol != '.')
                throw std::invalid_argument("Invalid light state");
            vectorLightDiagram.push_back(iLightSymbol == '#' ? 1 : 0);
        }

        // joltage
        std::vector<int> vectorJoltageRequirements;
        auto uJoltageOpen = stringLine.find('{');
        auto uJoltageClose = stringLine.find('}');
        if (uJoltageOpen == std::string::npos || uJoltageClose == std::string::npos ||
            uJoltageOpen <= uLightsClose || uJoltageClose <= uJoltageOpen)
            throw std::invalid_argument("Missing machine joltage");
        vectorJoltageRequirements =
            ParseIntegerList(stringLine.substr(uJoltageOpen, uJoltageClose - uJoltageOpen + 1));

        // buttons
        std::vector<std::vector<int>> vectorButtonWirings;
        auto stringWiring = core::Trim(
            std::string_view(stringLine).substr(uLightsClose + 1, uJoltageOpen - uLightsClose - 1));

        std::size_t uButtonOpen = 0;
        while (!stringWiring.empty()) {
            if (stringWiring.front() != '(')
                throw std::invalid_argument("Expected a machine button");
            auto uButtonClose = stringWiring.find(')', uButtonOpen);
            if (uButtonClose == std::string::npos)
                throw std::invalid_argument("Unclosed machine button");
            vectorButtonWirings.push_back(
                ParseIntegerList(stringWiring.substr(uButtonOpen, uButtonClose - uButtonOpen + 1)));
            stringWiring = core::Trim(stringWiring.substr(uButtonClose + 1));
        }

        if (!core::Trim(std::string_view(stringLine).substr(0, uLightsOpen)).empty() ||
            !core::Trim(std::string_view(stringLine).substr(uJoltageClose + 1)).empty())
            throw std::invalid_argument("Unexpected text around machine");
        if (vectorLightDiagram.empty() ||
            vectorLightDiagram.size() != vectorJoltageRequirements.size() ||
            std::ranges::any_of(vectorJoltageRequirements,
                                [](int iRequiredJoltage) { return iRequiredJoltage < 0; }))
            throw std::invalid_argument("Invalid machine targets");
        // A wiring index names an indicator light in part 1 and a joltage counter in part 2.
        for (auto& vectorWiring : vectorButtonWirings) {
            std::ranges::sort(vectorWiring);
            if (std::ranges::any_of(vectorWiring,
                                    [&](int iWiredIndex) {
                                        return iWiredIndex < 0 ||
                                               iWiredIndex >=
                                                   static_cast<int>(vectorLightDiagram.size());
                                    }) ||
                std::adjacent_find(vectorWiring.begin(), vectorWiring.end()) != vectorWiring.end())
                throw std::invalid_argument("Invalid machine button index");
        }
        // Repeated or empty buttons cannot improve a minimum-press solution.
        std::erase_if(vectorButtonWirings,
                      [](const auto& vectorWiring) { return vectorWiring.empty(); });
        std::ranges::sort(vectorButtonWirings);
        vectorButtonWirings.erase(
            std::unique(vectorButtonWirings.begin(), vectorButtonWirings.end()),
            vectorButtonWirings.end());
        m_vectorMachines.push_back({std::move(vectorLightDiagram),
                                    std::move(vectorJoltageRequirements),
                                    std::move(vectorButtonWirings)});
    }
}

// ------------------------------------------------------------
// Part 1 — GF(2) Gaussian elimination
// ------------------------------------------------------------

int Day10::FewestPressesForLights(const MachineDefinition& machinedefinition) {
    int iLightCount = static_cast<int>(machinedefinition.m_vectorLightDiagram.size());
    int iButtonCount = static_cast<int>(machinedefinition.m_vectorButtonWirings.size());

    std::vector<std::vector<int>> vectorLightEquations(iLightCount,
                                                       std::vector<int>(iButtonCount + 1, 0));
    for (int iLightIndex = 0; iLightIndex < iLightCount; ++iLightIndex)
        vectorLightEquations[iLightIndex][iButtonCount] =
            machinedefinition.m_vectorLightDiagram[iLightIndex];

    for (int iButtonColumn = 0; iButtonColumn < iButtonCount; ++iButtonColumn)
        for (int iLightIndex : machinedefinition.m_vectorButtonWirings[iButtonColumn])
            if (iLightIndex < iLightCount)
                vectorLightEquations[iLightIndex][iButtonColumn] = 1;

    int iPivotRow = 0;
    std::vector<int> vectorPivotRowByColumn(iButtonCount, -1);

    for (int iPivotColumn = 0; iPivotColumn < iButtonCount && iPivotRow < iLightCount;
         ++iPivotColumn) {
        int iSelectedRow = -1;
        for (int iRow = iPivotRow; iRow < iLightCount; ++iRow) {
            if (vectorLightEquations[iRow][iPivotColumn]) {
                iSelectedRow = iRow;
                break;
            }
        }
        if (iSelectedRow == -1)
            continue;

        std::swap(vectorLightEquations[iPivotRow], vectorLightEquations[iSelectedRow]);
        vectorPivotRowByColumn[iPivotColumn] = iPivotRow;

        for (int iRow = 0; iRow < iLightCount; ++iRow) {
            if (iRow != iPivotRow && vectorLightEquations[iRow][iPivotColumn]) {
                for (int iCoefficientColumn = iPivotColumn; iCoefficientColumn <= iButtonCount;
                     ++iCoefficientColumn)
                    vectorLightEquations[iRow][iCoefficientColumn] ^=
                        vectorLightEquations[iPivotRow][iCoefficientColumn];
            }
        }
        iPivotRow++;
    }

    for (int iRow = iPivotRow; iRow < iLightCount; ++iRow) {
        if (vectorLightEquations[iRow][iButtonCount] != 0)
            throw std::runtime_error("Unreachable light target");
    }

    std::vector<int> vectorFreeButtonColumns;
    for (int iButtonColumn = 0; iButtonColumn < iButtonCount; ++iButtonColumn)
        if (vectorPivotRowByColumn[iButtonColumn] == -1)
            vectorFreeButtonColumns.push_back(iButtonColumn);

    int iMinimumPressCount = iButtonCount + 1;
    std::vector<int> vectorPressParityByButton(iButtonCount, 0);
    const auto search_parities_ = [&](auto&& recurse_, std::size_t uFreeColumnIndex,
                                      int iPressCount) -> void {
        if (iPressCount >= iMinimumPressCount)
            return;
        if (uFreeColumnIndex < vectorFreeButtonColumns.size()) {
            const int iFreeColumn = vectorFreeButtonColumns[uFreeColumnIndex];
            vectorPressParityByButton[iFreeColumn] = 0;
            recurse_(recurse_, uFreeColumnIndex + 1, iPressCount);
            vectorPressParityByButton[iFreeColumn] = 1;
            recurse_(recurse_, uFreeColumnIndex + 1, iPressCount + 1);
            return;
        }
        for (int iButtonColumn = iButtonCount - 1; iButtonColumn >= 0; --iButtonColumn) {
            if (vectorPivotRowByColumn[iButtonColumn] == -1)
                continue;
            const int iRow = vectorPivotRowByColumn[iButtonColumn];
            int iPressParity = vectorLightEquations[iRow][iButtonCount];
            for (int iCoefficientColumn = iButtonColumn + 1; iCoefficientColumn < iButtonCount;
                 ++iCoefficientColumn)
                iPressParity ^= vectorLightEquations[iRow][iCoefficientColumn] &
                                vectorPressParityByButton[iCoefficientColumn];
            vectorPressParityByButton[iButtonColumn] = iPressParity;
            iPressCount += iPressParity;
        }
        iMinimumPressCount = std::min(iMinimumPressCount, iPressCount);
    };
    search_parities_(search_parities_, 0, 0);

    return iMinimumPressCount;
}

// ------------------------------------------------------------
// Part 2 — exact integer recursion on the binary digits of press counts
// ------------------------------------------------------------

std::int64_t Day10::FewestPressesForJoltage(const MachineDefinition& machinedefinition) {
    struct CounterVectorHash {
        std::size_t operator()(const std::vector<int>& vectorCounterValues) const noexcept {
            std::size_t uHashValue = 0;
            for (int iCounterValue : vectorCounterValues)
                uHashValue ^= std::hash<int>{}(iCounterValue) + 0x9e3779b9U + (uHashValue << 6) +
                              (uHashValue >> 2);
            return uHashValue;
        }
    };
    struct ParityChoice {
        std::vector<int> m_vectorCounterIncrements;
        int m_iPressCount;
    };

    const auto uCounterCount = machinedefinition.m_vectorJoltageRequirements.size();
    std::unordered_map<std::vector<int>, std::vector<ParityChoice>, CounterVectorHash>
        mapChoicesByParity;
    std::vector<int> vectorCounterIncrements(uCounterCount, 0);
    // Enumerate every set of buttons pressed an odd number of times.
    const auto enumerate_odd_presses_ = [&](auto&& recurse_, std::size_t uButtonIndex,
                                            int iPressCount) -> void {
        if (uButtonIndex == machinedefinition.m_vectorButtonWirings.size()) {
            auto vectorIncrementParity = vectorCounterIncrements;
            for (auto& iCounterValue : vectorIncrementParity)
                iCounterValue %= 2;
            mapChoicesByParity[vectorIncrementParity].push_back(
                {vectorCounterIncrements, iPressCount});
            return;
        }
        recurse_(recurse_, uButtonIndex + 1, iPressCount);
        for (int iCounterIndex : machinedefinition.m_vectorButtonWirings[uButtonIndex])
            ++vectorCounterIncrements[iCounterIndex];
        recurse_(recurse_, uButtonIndex + 1, iPressCount + 1);
        for (int iCounterIndex : machinedefinition.m_vectorButtonWirings[uButtonIndex])
            --vectorCounterIncrements[iCounterIndex];
    };
    enumerate_odd_presses_(enumerate_odd_presses_, 0, 0);

    using PressCountResult = std::optional<std::int64_t>;
    std::unordered_map<std::vector<int>, PressCountResult, CounterVectorHash>
        mapMinimumPressesByRemainingJoltage;
    const auto minimum_presses_ =
        [&](auto&& recurse_, const std::vector<int>& vectorRemainingJoltage) -> PressCountResult {
        if (std::ranges::all_of(vectorRemainingJoltage,
                                [](int iCounterValue) { return iCounterValue == 0; }))
            return 0;
        if (const auto itCachedPressCount =
                mapMinimumPressesByRemainingJoltage.find(vectorRemainingJoltage);
            itCachedPressCount != mapMinimumPressesByRemainingJoltage.end())
            return itCachedPressCount->second;

        auto vectorRemainingParity = vectorRemainingJoltage;
        for (auto& iCounterValue : vectorRemainingParity)
            iCounterValue %= 2;
        const auto itMatchingChoices = mapChoicesByParity.find(vectorRemainingParity);
        PressCountResult optionalMinimumPressCount;
        if (itMatchingChoices != mapChoicesByParity.end()) {
            for (const auto& paritychoice : itMatchingChoices->second) {
                std::vector<int> vectorHalvedJoltage(uCounterCount);
                bool bFitsRemainingJoltage = true;
                for (std::size_t uCounterIndex = 0; uCounterIndex < uCounterCount;
                     ++uCounterIndex) {
                    if (paritychoice.m_vectorCounterIncrements[uCounterIndex] >
                        vectorRemainingJoltage[uCounterIndex]) {
                        bFitsRemainingJoltage = false;
                        break;
                    }
                    vectorHalvedJoltage[uCounterIndex] =
                        (vectorRemainingJoltage[uCounterIndex] -
                         paritychoice.m_vectorCounterIncrements[uCounterIndex]) /
                        2;
                }
                if (!bFitsRemainingJoltage)
                    continue;
                // Each press adds at most one to any counter.
                const auto iPressCountLowerBound =
                    paritychoice.m_iPressCount +
                    2 * std::int64_t{std::ranges::max(vectorHalvedJoltage)};
                if (optionalMinimumPressCount &&
                    iPressCountLowerBound >= *optionalMinimumPressCount)
                    continue;
                if (const auto optionalRemainingPressCount =
                        recurse_(recurse_, vectorHalvedJoltage)) {
                    const auto iTotalPresses =
                        paritychoice.m_iPressCount + 2 * *optionalRemainingPressCount;
                    if (!optionalMinimumPressCount || iTotalPresses < *optionalMinimumPressCount)
                        optionalMinimumPressCount = iTotalPresses;
                }
            }
        }
        mapMinimumPressesByRemainingJoltage.emplace(vectorRemainingJoltage,
                                                    optionalMinimumPressCount);
        return optionalMinimumPressCount;
    };

    // Any press vector is uniquely x = odd + 2 * rest. Matching target parity
    // makes (target - A * odd) / 2 an exact, smaller integer subproblem.
    const auto optionalMinimumPressCount =
        minimum_presses_(minimum_presses_, machinedefinition.m_vectorJoltageRequirements);
    if (!optionalMinimumPressCount)
        throw std::runtime_error("Unreachable joltage target");
    return *optionalMinimumPressCount;
}

// ------------------------------------------------------------
// Day interface
// ------------------------------------------------------------

std::string Day10::Part1() {
    const std::int64_t iTotalPresses = core::ParallelSumIndexed(
        m_vectorMachines.size(), [&](std::size_t uMachineIndex) -> std::int64_t {
            const auto& machinedefinition = m_vectorMachines[uMachineIndex];
            if (machinedefinition.m_vectorLightDiagram.empty())
                return 0;
            return static_cast<std::int64_t>(FewestPressesForLights(machinedefinition));
        });

    return std::to_string(iTotalPresses);
}

std::string Day10::Part2() {
    const std::int64_t iTotalPresses = core::ParallelSumIndexed(
        m_vectorMachines.size(), [&](std::size_t uMachineIndex) -> std::int64_t {
            const auto& machinedefinition = m_vectorMachines[uMachineIndex];
            if (machinedefinition.m_vectorJoltageRequirements.empty())
                return 0;
            return static_cast<std::int64_t>(FewestPressesForJoltage(machinedefinition));
        });

    return std::to_string(iTotalPresses);
}
