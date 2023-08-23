#include <iostream>
#include <string>
#include <filesystem>
#include "philosophers.h"
#include "dinner_checker.h"
#include "GLOBALS.h"

int main(int argc, char** argv) {
    std::cout << argv[0] << " " << argv[1] << "\n";
    if (!(std::filesystem::exists(projectDir) && std::filesystem::is_directory(projectDir))) {
        std::cout << "projectDir is not configured properly.";
        exit(-1);
    }
    if (!std::filesystem::exists(projectDir + "dinners")) {
        std::filesystem::create_directory(projectDir + "dinners");
    }
    if(argc != 8){
        std::cout << "Wrong number of arguments passed.\n";
        std::cout << "Right format is : main dinId, genDinner, numPhilo, dinnerTime, timeToEat, timeToThink, chkDinner.\n";
        exit(-1);
    } else {
        bool genDinner;
        int numPhilo;
        int dinnerTime;
        int timeToEat;
        int timeToThink;
        bool chkDinner;
        std::string tempGen = argv[2];
        std::string tempChk = argv[7];
        try {
            numPhilo = std::stoi(argv[3]);
            dinnerTime = std::stoi(argv[4]);
            timeToEat = std::stoi(argv[5]);
            timeToThink = std::stoi(argv[6]);
            if(numPhilo < 2 || dinnerTime <= 0 || timeToEat <= 0 || timeToThink <= 0){
                throw std::exception();
            }
            if(!(tempGen == "true" || tempGen == "false") || !(tempChk == "true" || tempChk == "false")){
                throw std::exception();
            } else {
                genDinner = tempGen == "true";
                chkDinner = tempChk == "true";
            }
        } catch (...) {
            std::cout << "Invalid parameters entered";
            exit(-1);
        }
        if(genDinner){
            dinner(argv[1], numPhilo, dinnerTime, timeToEat, timeToThink);
        }
        if(chkDinner){
            std::string filePath = projectDir + "dinners/" + argv[1] + "_log";

            if (!std::filesystem::exists(filePath)) {
                std::cout << "Dinner is not yet generated.\n";
            } else {
                checkDinner(argv[1]);
            }
        }
    }

    return 0;
}
