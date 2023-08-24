#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include "GLOBALS.h"

int getNumberOfPhilosopher(std::ifstream& logFile){
    int highestID = -1;
    std::string line;

    while (std::getline(logFile, line)) {
        std::istringstream iss(line);
        std::string word;

        for (int i = 0; i < 4; ++i) {
            iss >> word;
        }

        if (!word.empty() && word[0] == 'P') {
            int id = std::stoi(word.substr(1));
            highestID = std::max(highestID, id);
        }
    }
    return highestID;
}

int getID(const std::string& id){
    return std::stoi(id.substr(1));
}

void isInteractionLegal(const std::string& philosopher, const std::string& chopstick, int maxPhilID){
    int philID { getID(philosopher) };
    int chopID { getID(chopstick) };
    if(!(chopID == philID || chopID == ((philID + 1) % (maxPhilID + 1)))){
        std::cout << "Illegal: " << philosopher << " interacted with chopstick " << chopstick << ".\n";
    }

}

void recordChopInteractions(const std::vector<std::string> &words, int maxPhilID,
                            std::map<std::string, std::map<std::string, std::vector<int>>>& chopInteractions) {
    const std::string& philosopher = words[3];
    const std::string& action = words[4];
    const std::string& chopstick = words[5];

    auto& philosopherMap = chopInteractions[philosopher];
    auto& interactions = philosopherMap[chopstick];

    if (interactions.empty()) {
        interactions = std::vector<int>(3, 0);
    }

    if (action == "PICKS") {
        interactions[0]++;
        isInteractionLegal(philosopher, chopstick, maxPhilID);
    } else if (action == "FAILS_PICK") {
        interactions[1]++;
        isInteractionLegal(philosopher, chopstick, maxPhilID);
    } else if (action == "DROPS") {
        interactions[2]++;
        isInteractionLegal(philosopher, chopstick, maxPhilID);
    } else {
        // Ignore
    }

}

void twoChopsticksInHand(const std::vector<std::string>& words, std::map<std::string, int>& numChopsticks,
                         std::map<std::string, std::vector<int>>& eatCount) {
    const std::string& philosopher = words[3];
    const std::string& action = words[4];

    if (action == "PICKS") {
        ++numChopsticks[philosopher];
    } else if (action == "DROPS") {
        --numChopsticks[philosopher];
    } else if(action == "STARTS_EATING") {
        int numChops { numChopsticks[philosopher] };
        if(numChops != 2){
            std::cout << "Error: " << philosopher << " ate with " << numChops << " in hand.\n";
        } else {
            eatCount[philosopher].push_back(numChops);
        }
    }
}

void updateChopState(std::vector<std::string> words, std::map<std::string, bool>& chopTaken) {
    const std::string& action = words[4];
    const std::string& chopstick = words[5];
    if(action == "PICKS"){
        if(chopTaken[chopstick]){
            std::cout << "Error: " << chopstick << " is already taken.\n";
        } else {
            chopTaken[chopstick] = true;
        }
    } else if(action == "DROPS"){
        if(!chopTaken[chopstick]){
            std::cout << "Error: " << chopstick << " is already dropped.\n";
        } else {
            chopTaken[chopstick] = false;
        }
    }

}

int checkDinner(const std::string& dinner_id) {
    std::cout << "Starting review of dinner: " << dinner_id << ".\n";
    std::ifstream logFile(projectDir + "dinners/" + dinner_id + "_log");
    std::ifstream logFileCopy(projectDir + "dinners/" + dinner_id + "_log");
    std::ofstream reviewFile(projectDir + "dinners/" + dinner_id + "_review", std::ios::trunc);

    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file.\n";
        return 1;
    }

    if (!reviewFile.is_open()) {
        std::cerr << "Failed to open log file.\n";
        return 1;
    }

    int numberOfPhils { getNumberOfPhilosopher(logFileCopy) };
    std::cout << "There were " << numberOfPhils << " philosophers at the dinner.\n";
    logFileCopy.close();

    std::map<std::string, std::map<std::string, std::vector<int>>> chopInteractions {};
    std::map<std::string, int> numChopSticks;
    std::map<std::string, std::vector<int>> eatCount;
    std::map<std::string, bool> chopstickTaken;

    std::string line;
    while (std::getline(logFile, line)) {
        if (!line.empty()) {
            std::istringstream iss(line);
            std::string word;
            std::vector<std::string> words;

            while (iss >> word) {
                words.push_back(word);
            }

            updateChopState(words, chopstickTaken);
            recordChopInteractions(words, numberOfPhils, chopInteractions);
            twoChopsticksInHand(words, numChopSticks, eatCount);
        }
    }

    for (const auto& philosopherEntry : chopInteractions) {
        const std::string& philosopher = philosopherEntry.first;
        const std::map<std::string, std::vector<int>>& chopstickMap = philosopherEntry.second;

        reviewFile << philosopher << ":\n";

        for (const auto& chopstickEntry : chopstickMap) {
            const std::string& chopstick = chopstickEntry.first;
            const std::vector<int>& interactions = chopstickEntry.second;

            reviewFile << chopstick << " -> ";
            reviewFile << "Picked: " << interactions[0] << ", ";
            reviewFile << "Failed pick: " << interactions[1] << ", ";
            reviewFile << "Dropped: " << interactions[2] <<"\n";
        }
    }

    logFile.close();
    reviewFile.close();

    return 0;
}

