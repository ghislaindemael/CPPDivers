#ifndef DINNER_H
#define DINNER_H

#include <vector>
#include <memory>
#include "ThreadSafeLogger.h"
#include "Chopstick.h"
#include "Philosopher.h"
#include "Arguments.h"
class Dinner {
public:
    explicit Dinner(const Arguments& args);

    void run();

private:
    Arguments m_arguments;
    std::shared_ptr<ThreadSafeLogger> m_loggerPtr;
    std::vector<std::shared_ptr<Chopstick>> m_chopsticks;
    std::vector<std::unique_ptr<Philosopher>> m_philosophers;
    int m_dinnerTime;
    std::string m_projectDir;
};

#endif // DINNER_H
