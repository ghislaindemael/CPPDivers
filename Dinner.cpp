#include "Dinner.h"
#include <iostream>
#include <chrono>
#include <thread>

Dinner::Dinner(const Arguments& args)
        : m_arguments(args),
          m_loggerPtr(std::make_shared<ThreadSafeLogger>(
                  args.getArgs().projectDir + "dinners/" + args.getArgs().dinnerId)),
          m_dinnerTime(args.getArgs().dinnerTime), m_projectDir(args.getArgs().projectDir) {

    m_loggerPtr->logNumberOfPhilosophers(args.getArgs().numPhilo);

    m_chopsticks.reserve(args.getArgs().numPhilo);
    for (int i = 0; i < args.getArgs().numPhilo; ++i) {
        m_chopsticks.push_back(std::make_shared<Chopstick>(i));
    }

    m_philosophers.reserve(args.getArgs().numPhilo);
    for (int i = 0; i < args.getArgs().numPhilo; ++i) {
        m_philosophers.push_back(std::make_unique<Philosopher>(
                i, args.getArgs().timeToEat, args.getArgs().timeToThink, m_chopsticks[i], m_chopsticks[(i + 1) % args.getArgs().numPhilo],
                m_loggerPtr));
    }
}

void Dinner::run() {
    std::this_thread::sleep_for(std::chrono::seconds(m_dinnerTime));
    for (const auto & m_philosopher : m_philosophers) {
        m_philosopher->leaveTable();
    }

    for (const auto & m_philosopher : m_philosophers) {
        m_philosopher->joinThread();
    }

    std::cout << "Dinner ended.\n";
}
