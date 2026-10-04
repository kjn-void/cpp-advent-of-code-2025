#include "core/Input.h"
#include "core/Parse.h"
#include "core/Registry.h"
#include "core/Solution.h"
#include <iostream>

int main(int iArgumentCount, char** ppbszArgument) {
    if (iArgumentCount < 2) {
        std::cerr << "Usage: aoc2025 DAY [DAY ...]\n";
        return 1;
    }

    int iExitCode = 0;

    for (int iArgumentIndex = 1; iArgumentIndex < iArgumentCount; iArgumentIndex++) {
        int iDay = 0;
        try {
            iDay = core::ParseInteger<int>(ppbszArgument[iArgumentIndex]);
        } catch (const std::exception& exceptionError) {
            std::cerr << "Invalid day argument '" << ppbszArgument[iArgumentIndex]
                      << "': " << exceptionError.what() << "\n";
            iExitCode = 1;
            continue;
        }

        auto psolution = Registry::Instance().Make(iDay);
        if (!psolution) {
            std::cerr << "Day " << iDay << " not implemented\n";
            iExitCode = 1;
            continue;
        }

        try {
            auto vectorInputLines = core::ReadInput(iDay);
            psolution->SetInput(vectorInputLines);

            const auto stringPart1Answer = psolution->Part1();
            const auto stringPart2Answer = psolution->Part2();
            std::cout << "Day " << iDay << "\n";
            std::cout << "  Part 1: " << stringPart1Answer << "\n";
            std::cout << "  Part 2: " << stringPart2Answer << "\n";
        } catch (const std::exception& exceptionError) {
            std::cerr << "Day " << iDay << " failed: " << exceptionError.what() << "\n";
            iExitCode = 1;
        }
    }

    return iExitCode;
}
