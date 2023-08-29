#ifndef THREADSAFELOGGER_H
#define THREADSAFELOGGER_H

#include <string>
#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include "Chopstick.h"

class ThreadSafeLogger {
public:
    explicit ThreadSafeLogger(const std::string& filename);
    ~ThreadSafeLogger();

    std::string getTimeDiff();
    static std::string formatPID(int p_id);
    static std::string formatCID(const std::shared_ptr<Chopstick>& chop);

    void logNumberOfPhilosophers(int n_phil);
    void logSeatAtTable(int p_id);
    void logLeavesTable(int p_id);
    void logStartEating(int p_id);
    void logFinishEating(int p_id);
    void logStartThinking(int p_id);
    void logFinishThinking(int p_id);
    void logPickupChopstick(int p_id, const std::shared_ptr<Chopstick>& chop, bool success);
    void logDropChopstick(int p_id, const std::shared_ptr<Chopstick>& chop);

private:
    void logLineWithTS(const std::string& s);
    void logWorker();

    std::ofstream m_logFile;
    std::thread m_logThread;
    std::mutex m_logMutex;
    std::condition_variable m_logCondition;
    std::queue<std::string> m_logQueue;
    bool m_logStopFlag;
    std::chrono::time_point<std::chrono::system_clock> m_baseTime;
};

#endif // THREADSAFELOGGER_H
