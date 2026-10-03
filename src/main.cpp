#include "core/Input.h"
#include "core/Parse.h"
#include "core/Registry.h"
#include "core/Solution.h"
#include <iostream>

int main(int cusArgs, char** rgusArgs) {
    if (cusArgs < 2) {
        std::cerr << "Usage: aoc2025 DAY [DAY ...]\n";
        return 1;
    }

    int rcProgram = 0;

    for (int iusArg = 1; iusArg < cusArgs; iusArg++) {
        int idDay = 0;
        try {
            idDay = core::ValParseInteger<int>(rgusArgs[iusArg]);
        } catch (const std::exception& errFailure) {
            std::cerr << "Invalid day argument '" << rgusArgs[iusArg] << "': " << errFailure.what()
                      << "\n";
            rcProgram = 1;
            continue;
        }

        auto pslvDay = Regy::RegyInstance().PslvMake(idDay);
        if (!pslvDay) {
            std::cerr << "Day " << idDay << " not implemented\n";
            rcProgram = 1;
            continue;
        }

        try {
            auto rgusLines = core::RgusReadInput(idDay);
            pslvDay->SetInput(rgusLines);

            const auto txtPart1 = pslvDay->TxtPart1();
            const auto txtPart2 = pslvDay->TxtPart2();
            std::cout << "Day " << idDay << "\n";
            std::cout << "  Part 1: " << txtPart1 << "\n";
            std::cout << "  Part 2: " << txtPart2 << "\n";
        } catch (const std::exception& errFailure) {
            std::cerr << "Day " << idDay << " failed: " << errFailure.what() << "\n";
            rcProgram = 1;
        }
    }

    return rcProgram;
}
