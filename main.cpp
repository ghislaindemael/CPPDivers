#include <iostream>
#include <string>
#include <filesystem>
#include "philosophers.h"
#include "dinner_checker.h"
#include "GLOBALS.h"

int main(int argc, char** argv) {
    std::cout << argv[0] << " " << argv[1] << "\n";
    if(argc != 6){
        std::cout << "Wrong number of arguments passed.\n";
        std::cout << "Right format is : main dinId, genDinner, numPhilo, timeToEat, chkDinner,\n";
        std::cout << "where dinId, numPhilo and timeToEat are of type int and genDinner, chkDinner are booleans.\n";
    } else {
        int numPhilo;
        int timeToEat;
        bool genDinner;
        bool chkDinner;
        std::string tempGen = argv[2];
        std::string tempChk = argv[5];
        try {
            numPhilo = std::stoi(argv[3]);
            timeToEat = std::stoi(argv[4]);
            if(!(tempGen == "true" || tempGen == "false") || !(tempChk == "true" || tempChk == "false")){
                std::cout << "Error converting the booleans.\n";
                throw std::exception();
            } else {
                genDinner = tempGen == "true";
                chkDinner = tempChk == "true";
            }
        } catch (...) {
            std::cout << "Invalid parameters entered";
        }
        if(genDinner){
            dinner(argv[1], numPhilo, timeToEat);
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
