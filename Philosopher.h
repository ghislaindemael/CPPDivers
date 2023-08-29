#ifndef PHILOSOPHER_H
#define PHILOSOPHER_H

#include <memory>
#include <thread>
#include "Chopstick.h"
#include "ThreadSafeLogger.h"

class Philosopher {
private:
    int m_id;
    bool m_isSeated { true };
    int m_timeToEat;
    int m_timeToThink;
    std::shared_ptr<Chopstick> m_leftChop;
    std::shared_ptr<Chopstick> m_rightChop;
    std::shared_ptr<ThreadSafeLogger> m_logger;
    std::thread m_mainThread;

public:
    Philosopher(int i_id, int i_tteat, int i_ttthink, std::shared_ptr<Chopstick> i_lchop,
                std::shared_ptr<Chopstick> i_rchop, std::shared_ptr<ThreadSafeLogger> i_logptr);

    void leaveTable();
    void joinThread();
    void eatSequence();
    void thinkSequence();
    void dropChopstick(const std::shared_ptr<Chopstick>& chopstick);
    void dropChopsticks();
    bool pickupChopstick(const std::shared_ptr<Chopstick>& chopstick);
    void dine();
};

#endif // PHILOSOPHER_H
