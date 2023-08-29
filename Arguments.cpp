#include "Arguments.h"
#include <iostream>
#include <cmath>

Arguments::Arguments(int argc, char** argv) : valid(false) {
    if (argc != 8) {
        std::cerr << "Usage: " << argv[0] << " <dinner_id> <genDinner> <numPhilo> <dinnerTime> <timeToEat> <chkDinner> <projectDir>\n";
        return;
    }

    if (!parseArgs(argv)) {
        std::cerr << "Invalid args.\n";
        return;
    }

    valid = true;
}

bool Arguments::parseArgs(char **argv) {
    args.dinnerId = argv[1];
    args.generateDinner = (std::string(argv[2]) == "true");
    args.numPhilo = std::stoi(argv[3]);
    args.dinnerTime = std::stoi(argv[4]);
    args.timeToEat = std::stoi(argv[5]);
    args.checkDinner = (std::string(argv[6]) == "true");
    args.projectDir = argv[7];

    int timeToThink = static_cast<int>(std::round(1.1 * args.timeToEat));

    if (args.numPhilo < 2 || args.dinnerTime <= 0 || args.timeToEat <= 0 || timeToThink <= 0) {
        return false;
    }

    return true;
}
