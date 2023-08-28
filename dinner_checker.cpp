#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <regex>
#include "GLOBALS.h"

namespace {
    enum PossibleStates {
        RESTING,
        THINKING,
        EATING,
        NOT_SEATED
    };
}

bool isLogLineValid(const std::string& line, std::regex& pattern) {
    return std::regex_match(line, pattern);
}


int getNumberOfPhilosopher(std::ifstream& logFile, std::regex& pattern){
    int highestID = -1;
    std::string line;

    while (std::getline(logFile, line)) {
        if(isLogLineValid(line, pattern)) {
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

void recordChopInteractions(const std::vector<std::string>& words, int maxPhilID,
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

bool twoChopsticksInHand(const std::vector<std::string>& words, std::map<std::string, int>& numChopsticks,
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
            return false;
        } else {
            eatCount[philosopher].push_back(numChops);
        }
    }
    return true;
}

bool updateChopState(std::vector<std::string>& words, std::map<std::string, bool>& chopTaken) {
    const std::string& action = words[4];
    const std::string& chopstick = words[5];
    if(action == "PICKS"){
        if(chopTaken[chopstick]){
            std::cout << "Error: " << chopstick << " is already taken.\n";
            return false;
        } else {
            chopTaken[chopstick] = true;
        }
    } else if(action == "DROPS"){
        if(!chopTaken[chopstick]){
            std::cout << "Error: " << chopstick << " is already dropped.\n";
            return false;
        } else {
            chopTaken[chopstick] = false;
        }
    }
    return true;
}

bool updatePhiloState(std::vector<std::string>& words, std::map<std::string, PossibleStates>& states) {
    const std::string& philosopher = words[3];
    const std::string& action = words[4];
    if(action == "SEATS"){
        states[philosopher] = PossibleStates::RESTING;
    } else if(action == "STARTS_EATING"){
        if(states[philosopher] != PossibleStates::RESTING){
            std::cout << "Error: " << philosopher << " start eating while eating or thinking.\n";
            return false;
        } else {
            states[philosopher] = PossibleStates::EATING;
        }
    } else if(action == "STOPS_EATING"){
        if(states[philosopher] != PossibleStates::EATING){
            std::cout << "Error: " << philosopher << " stops eating while not eating.\n";
            return false;
        } else {
            states[philosopher] = PossibleStates::RESTING;
        }
    } else if(action == "STARTS_THINKING"){
        if(states[philosopher] != PossibleStates::RESTING){
            std::cout << "Error: " << philosopher << " starts thinking while eating or thinking.\n";
            return false;
        } else {
            states[philosopher] = PossibleStates::THINKING;
        }
    } else if(action == "STOPS_THINKING"){
        if(states[philosopher] != PossibleStates::THINKING){
            std::cout << "Error: " << philosopher << " stops thinking while not thinking.\n";
            return false;
        } else {
            states[philosopher] = PossibleStates::RESTING;
        }
    } else if(action == "LEAVES"){
        if(states[philosopher] != PossibleStates::RESTING){
            std::cout << "Error: " << philosopher << " left while eating or thinking.\n";
            return false;
        }
    }
    return true;
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

    std::regex pattern(R"(\s*(\d+)\s*ms\s*\|\s*P(\d+)\s+(SEATS|LEAVES|PICKS|FAILS_PICK|DROPS|STARTS_THINKING|STOPS_THINKING|STARTS_EATING|STOPS_EATING)\s*(C\d+|XX)?\s*)");

    int numberOfPhils { getNumberOfPhilosopher(logFileCopy, pattern) };
    std::cout << "There were " << numberOfPhils << " philosophers at the dinner.\n";
    logFileCopy.close();

    std::map<std::string, std::map<std::string, std::vector<int>>> chopInteractions {};
    std::map<std::string, int> numChopSticks;
    std::map<std::string, std::vector<int>> eatCount;
    std::map<std::string, bool> chopstickTaken;
    std::map<std::string, PossibleStates> philoState;

    std::string line;
    int lineCount { 1 };

    while (std::getline(logFile, line)) {
        if (!line.empty()) {
            if(isLogLineValid(line, pattern)) {
                std::istringstream iss(line);
                std::string word;
                std::vector<std::string> words;

                while (iss >> word) {
                    words.push_back(word);
                }
                if(!updateChopState(words, chopstickTaken)
                || !updatePhiloState(words, philoState)
                || !twoChopsticksInHand(words, numChopSticks, eatCount)){
                    std::cout << "Log line in question : " << lineCount << "\n";
                }
                recordChopInteractions(words, numberOfPhils, chopInteractions);
            } else {
                std::cout << "Line " << lineCount << " is not a valid log line.\n";
            }
        }
        ++lineCount;
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
            reviewFile << "Dropped: " << interactions[2] <<", ";
            reviewFile << "Failed pick: " << interactions[1] << "\n";

        }
    }

    logFile.close();
    reviewFile.close();

    return 0;
}

