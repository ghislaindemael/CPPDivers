#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include "GLOBALS.h"

void chopstickInteractions(std::vector<std::vector<std::string>> &linesList, std::ofstream& reviewFile) {
    std::map<std::string, std::map<std::string, int>> failCount;
    std::map<std::string, std::map<std::string, int>> pickCount;
    std::map<std::string, std::map<std::string, int>> dropCount;

    for (const auto& lineWords : linesList) {
        std::string philosopher = lineWords[3];
        std::string action = lineWords[4];

        if (action == "picks") {
            ++pickCount[philosopher][lineWords[7]];
        } else if (action == "fails") {
            ++failCount[philosopher][lineWords[9]];
        } else if (action == "drops") {
            ++dropCount[philosopher][lineWords[6]];
        } else {
            //Ignore
        }
    }

    for (const auto& philosopherEntry : pickCount) {
        const std::string& philosopher = philosopherEntry.first;
        const std::map<std::string, int>& pickMap = philosopherEntry.second;
        const std::map<std::string, int>& dropMap = dropCount[philosopher];
        const std::map<std::string, int>& failMap = failCount[philosopher];

        reviewFile << philosopher << ":" << std::endl;
        for (const auto& pickEntry : pickMap) {
            const std::string& chopstick = pickEntry.first;
            int pickCount = pickEntry.second;

            auto dropCountIter = dropMap.find(chopstick);
            int dropCount = (dropCountIter != dropMap.end()) ? dropCountIter->second : 0;

            auto failCountIter = failMap.find(chopstick);
            int failCount = (failCountIter != failMap.end()) ? failCountIter->second : 0;

            reviewFile << " Chopstick " << chopstick << ": Picked up " << pickCount << ", Dropped " << dropCount << ", Failed " << failCount <<"\n";
        }
    }
}

void numChopsticksToEat(std::vector<std::vector<std::string>> &linesList, std::ofstream& reviewFile) {

    std::map<std::string, int> numChopSticks;
    std::map<std::string, std::vector<int>> thinkCount;
    std::map<std::string, std::vector<int>> eatCount;

    for (const auto& lineWords : linesList) {
        std::string philosopher = lineWords[3];
        std::string action = lineWords[4];

        if (action == "picks") {
            ++numChopSticks[philosopher];
        } else if (action == "drops") {
            --numChopSticks[philosopher];
        } else {
            //Ignore
        }

        if(lineWords.size() >= 7){
            std::string newAction = lineWords[6];
            if (newAction == "eat.") {
                eatCount[philosopher].push_back(numChopSticks[philosopher]);
            } else if (newAction == "think.") {
                thinkCount[philosopher].push_back(numChopSticks[philosopher]);
            }
        }
    }

    for (const auto& entry : eatCount) {
        const std::string& philosopher = entry.first;
        const std::vector<int>& chopsticks = entry.second;

        reviewFile << philosopher << " ate with:\t";
        for (int chopstick : chopsticks) {
            reviewFile << " " << chopstick;
            if(chopstick < 2){
                std::cout << "Error : ate with less than chopsticks in hand.";
                exit(-1);
            }
        }
        reviewFile << " chopsticks in hand.\n";
    }
    std::cout << "No illegal eating action detected.\n";

    for (const auto& entry : thinkCount) {
        const std::string& philosopher = entry.first;
        const std::vector<int>& chopsticks = entry.second;

        reviewFile << philosopher << " thought with\t";
        for (int chopstick : chopsticks) {
            reviewFile << " " << chopstick;
            if(chopstick > 0){
                std::cout << "Error : thought with chopsticks in hand.";
                exit(-1);
            }
        }
        reviewFile << " chopsticks in hand.\n";
    }
    std::cout << "No illegal thinking action detected.\n";
}

int checkDinner(const std::string& dinner_id) {
    std::cout << "Starting review of dinner: " << dinner_id << ".\n";
    std::ifstream logFile(projectDir + "dinners/" + dinner_id + "_log");
    std::ofstream reviewFile(projectDir + "dinners/" + dinner_id + "_review", std::ios::app);

    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file.\n";
        exit(-1);
    }

    if (!reviewFile.is_open()) {
        std::cerr << "Failed to open log file.\n";
        exit(-1);
    }

    std::vector<std::vector<std::string>> linesList;
    std::string line;

    while (std::getline(logFile, line)) {
        if (!line.empty()) {
            std::istringstream iss(line);
            std::string word;
            std::vector<std::string> words;

            while (iss >> word) {
                words.push_back(word);
            }
            linesList.push_back(words);
        }
    }

    logFile.close();

    chopstickInteractions(linesList, reviewFile);
    numChopsticksToEat(linesList, reviewFile);

    reviewFile.close();

    return 0;
}

