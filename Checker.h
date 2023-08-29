#ifndef CHECKER_H
#define CHECKER_H

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <regex>

class Checker {
public:
    Checker(std::string& i_dinnerId, std::string& i_projectDir);

    int checkDinner();

    enum PhiloStates {
        RESTING,
        THINKING,
        EATING,
        NOT_SEATED
    };
private:
    bool isLogLineValid(const std::string& line, std::regex& pattern);
    int getMaxPhilID(std::ifstream& logFile);
    int getID(const std::string& id);
    void isInteractionLegal(const std::string& philosopher, const std::string& chopstick, int maxPhilID);
    void recordChopInteractions(const std::vector<std::string>& words, int maxPhilID,
                                std::map<std::string, std::map<std::string, std::vector<int>>>& chopInteractions);
    bool twoChopsticksInHand(const std::vector<std::string>& words, std::map<std::string, int>& numChopsticks,
                             std::map<std::string, std::vector<int>>& eatCount);
    bool updateChopState(std::vector<std::string>& words, std::map<std::string, bool>& chopTaken);
    bool updatePhiloState(std::vector<std::string>& words, std::map<std::string, PhiloStates>& states);

    std::string projectDir;
    std::string m_dinnerId;
};

#endif // CHECKER_H
