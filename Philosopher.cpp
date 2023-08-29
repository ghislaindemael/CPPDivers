#include "Philosopher.h"

Philosopher::Philosopher(int i_id, int i_tteat, int i_ttthink, std::shared_ptr<Chopstick> i_lchop,
                         std::shared_ptr<Chopstick> i_rchop, std::shared_ptr<ThreadSafeLogger> i_logptr)
        : m_id(i_id), m_timeToEat(i_tteat), m_timeToThink(i_ttthink), m_leftChop(std::move(i_lchop)),
          m_rightChop(std::move(i_rchop)), m_logger(std::move(i_logptr)),
          m_mainThread(std::thread(&Philosopher::dine, this))
{
}

void Philosopher::leaveTable() {
    m_isSeated = false;
}

void Philosopher::joinThread() {
    m_mainThread.join();
}

void Philosopher::eatSequence() {
    m_logger->logStartEating(m_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(m_timeToEat));
    m_logger->logFinishEating(m_id);
}

void Philosopher::thinkSequence() {
    m_logger->logStartThinking(m_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(m_timeToThink));
    m_logger->logFinishThinking(m_id);
}

void Philosopher::dropChopstick(const std::shared_ptr<Chopstick>& chopstick) {
    m_logger->logDropChopstick(m_id, chopstick);
    chopstick->sem.release();
}

void Philosopher::dropChopsticks() {
    dropChopstick(m_leftChop);
    dropChopstick(m_rightChop);
}

bool Philosopher::pickupChopstick(const std::shared_ptr<Chopstick>& chopstick) {
    if (chopstick->sem.try_acquire()) {
        m_logger->logPickupChopstick(m_id, chopstick, true);
        return true;
    } else {
        m_logger->logPickupChopstick(m_id, chopstick, false);
        return false;
    }
}

void Philosopher::dine() {
    m_logger->logSeatAtTable(m_id);
    while (m_isSeated) {
        if (pickupChopstick(m_leftChop)) {
            if (pickupChopstick(m_rightChop)) {
                eatSequence();
                dropChopsticks();
                thinkSequence();
            } else {
                dropChopstick(m_leftChop);
                thinkSequence();
            }
        } else {
            thinkSequence();
        }
    }
    m_logger->logLeavesTable(m_id);
}
