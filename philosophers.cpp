#include <utility>
#include <vector>
#include <semaphore>
#include <thread>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <queue>

#include "GLOBALS.h"

using bin_sem = std::binary_semaphore;

struct Chopstick {
    int id;
    bin_sem sem;
    explicit Chopstick(int i_id) : id { i_id }, sem(bin_sem(1)){};
};

class ThreadSafeLogger {
    std::chrono::time_point<std::chrono::system_clock> m_baseTime;
public:
    explicit ThreadSafeLogger(const std::string& filename)
    : logFile(filename + "_log"), logStopFlag(false), m_baseTime { std::chrono::system_clock::now() }
    {
        logThread = std::thread(&ThreadSafeLogger::logWorker, this);
    }

    ~ThreadSafeLogger() {
        logStopFlag = true;
        logCondition.notify_one();
        logThread.join();
        logFile.close();
    }

    std::string getTimeDiff() {
        auto duration = std::chrono::system_clock::now() - m_baseTime;
        return std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) + " ms | ";
    }

    static std::string formatPID(int p_id) {
        return "P" + std::to_string(p_id);
    }

    static std::string formatCID(const std::shared_ptr<Chopstick>& chop){
        return "C" + std::to_string(chop->id);
    }

    void logSeatAtTable(int p_id){
        std::lock_guard<std::mutex> lock(logMutex);
        logQueue.push(getTimeDiff() + formatPID(p_id) + " takes a seat at the table.\n");
        logCondition.notify_one();
    }

    void logLeavesTable(int p_id){
        std::lock_guard<std::mutex> lock(logMutex);
        logQueue.push(getTimeDiff() + formatPID(p_id) + " leaves the table.\n");
        logCondition.notify_one();
    }

    void logStartEating(int p_id) {
        std::lock_guard<std::mutex> lock(logMutex);
        logQueue.push(getTimeDiff() + formatPID(p_id) + " starts to eat.\n");
        logCondition.notify_one();
    }

    void logFinishEating(int p_id) {
        std::lock_guard<std::mutex> lock(logMutex);
        logQueue.push(getTimeDiff() + formatPID(p_id) + " finished eating.\n");
        logCondition.notify_one();
    }

    void logStartThinking(int p_id) {
        std::lock_guard<std::mutex> lock(logMutex);
        logQueue.push(getTimeDiff() + formatPID(p_id) + " starts to think.\n");
        logCondition.notify_one();
    }

    void logFinishThinking(int p_id) {
        std::lock_guard<std::mutex> lock(logMutex);
        logQueue.push(getTimeDiff() + formatPID(p_id) + " finished thinking.\n");
        logCondition.notify_one();
    }

    void logPickupChopstick(int p_id, const std::shared_ptr<Chopstick>& chop, bool success){
        std::lock_guard<std::mutex> lock(logMutex);
        if(success){
            logQueue.push(getTimeDiff() + formatPID(p_id) + " picks up chopstick " + formatCID(chop) + ".\n");
        } else {
            logQueue.push(getTimeDiff() + formatPID(p_id) + " fails to pick up chopstick " + formatCID(chop) + ".\n");
        }
        logCondition.notify_one();
    }

    void logDropChopstick(int p_id, const std::shared_ptr<Chopstick>& chop){
        std::lock_guard<std::mutex> lock(logMutex);
        logQueue.push(getTimeDiff() + formatPID(p_id) + " drops chopstick " + formatCID(chop) + ".\n");
        logCondition.notify_one();
    }

private:
    void logWorker() {
        while (true) {
            std::unique_lock<std::mutex> lock(logMutex);
            logCondition.wait(lock, [this]() { return !logQueue.empty() || logStopFlag; });

            while (!logQueue.empty()) {
                logFile << logQueue.front() << std::endl;
                logQueue.pop();
            }

            if (logStopFlag) {
                break;
            }
        }
    }

    std::ofstream logFile;
    std::thread logThread;
    std::mutex logMutex;
    std::condition_variable logCondition;
    std::queue<std::string> logQueue;
    bool logStopFlag;
};

class Philosopher {
    int m_id;
    bool m_isSeated { true };
    int m_timesEaten { 0 };
    int m_timeToEat;
    int m_timeToThink;
    std::shared_ptr<Chopstick> m_left_chop;
    std::shared_ptr<Chopstick> m_right_chop;
    std::shared_ptr<ThreadSafeLogger> m_logger;
    std::thread m_mainThread;
public:
    Philosopher(int i_id, int i_tteat, int i_ttthink, std::shared_ptr<Chopstick> i_lchop,
                std::shared_ptr<Chopstick> i_rchop, std::shared_ptr<ThreadSafeLogger> i_logptr)
        : m_id {i_id }, m_timeToEat {i_tteat }, m_timeToThink { i_ttthink }, m_left_chop {std::move(i_lchop) },
            m_right_chop {std::move(i_rchop) }, m_logger { std::move(i_logptr) }, m_mainThread {std::thread(&Philosopher::dine, this) }
          {
              m_logger->logSeatAtTable(m_id);
          }

    void leaveTable(){
        m_isSeated = false;
    }
    void joinThread(){
        m_mainThread.join();
    }

    void eatSequence() {
        ++m_timesEaten;
        m_logger->logStartEating(m_id);
        std::this_thread::sleep_for (std::chrono::milliseconds(m_timeToEat));
        m_logger->logFinishEating(m_id);
    }
    void thinkSequence(){
        m_logger->logStartThinking(m_id);
        std::this_thread::sleep_for (std::chrono::milliseconds(m_timeToThink));
        m_logger->logFinishThinking(m_id);
    }

    void dropChopstick(const std::shared_ptr<Chopstick>& chopstick){
        m_logger->logDropChopstick(m_id, chopstick);
        chopstick->sem.release();
    }

    void dropChopsticks(){
        dropChopstick(m_left_chop);
        dropChopstick(m_right_chop);
    }

    bool pickupChopstick(const std::shared_ptr<Chopstick>& chopstick){
        if(chopstick->sem.try_acquire()){
            m_logger->logPickupChopstick(m_id, chopstick, true);
            return true;
        } else {
            m_logger->logPickupChopstick(m_id, chopstick, false);
            return false;
        }
    }
    void dine(){
        while (m_isSeated){
            if(pickupChopstick(m_left_chop)){
                if(pickupChopstick(m_right_chop)){
                    eatSequence();
                    dropChopsticks();
                    thinkSequence();
                } else {
                    dropChopstick(m_left_chop);
                    thinkSequence();
                }
            } else {
                thinkSequence();
            }
        }
        m_logger->logLeavesTable(m_id);
    }
};

int dinner(const std::string& dinner_id, int numPhilo, int timeToEat){
    {

        std::shared_ptr<ThreadSafeLogger> loggerPtr{
                std::make_shared<ThreadSafeLogger>(projectDir + "dinners/" + dinner_id)};

        std::vector<std::shared_ptr<Chopstick>> chopsticks;
        for (int i = 0; i < numPhilo; ++i) {
            chopsticks.push_back(std::make_shared<Chopstick>(i));
        }

        std::vector<std::unique_ptr<Philosopher>> philosophers;
        for (int i = 0; i < numPhilo; ++i) {
            philosophers.push_back(
                    std::make_unique<Philosopher>(i, timeToEat, 150, chopsticks[i], chopsticks[(i + 1) % numPhilo],
                                                  loggerPtr));
        }

        std::this_thread::sleep_for(std::chrono::seconds(10));
        for (int i = 0; i < numPhilo; ++i) {
            philosophers[i]->leaveTable();
        }

        for (int i = 0; i < numPhilo; ++i) {
            philosophers[i]->joinThread();
        }

    }
    return 0;
}