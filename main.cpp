#include <iostream>
#include "Arguments.h"
#include "Dinner.h"
#include "Checker.h"

int main(int argc, char** argv) {
    Arguments cmdArgs(argc, argv);

    if (!cmdArgs.isValid()) {
        std::cerr << "Invalid command-line args.\n";
        return EXIT_FAILURE;
    }

    const Arguments::Args& args = cmdArgs.getArgs();

    if (args.generateDinner) {
        Dinner dinner(args);
        dinner.run();
    }

    if (args.checkDinner) {
        Checker checker(args.dinnerId, args.projectDir);
        checker.checkDinner();
    }

    return EXIT_SUCCESS;
}
