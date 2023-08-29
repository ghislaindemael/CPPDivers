#include <string>

class Arguments {
public:

    Arguments(int argc, char** argv);
    bool isValid() const { return valid; }

private:
    std::string dinnerId;
    bool generateDinner;
    int numPhilo;
    int dinnerTime;
    int timeToEat;
    int timeToThink;
    bool checkDinner;
    std::string projectDir;
    bool valid;

    bool parseArgs(char** argv);
};
