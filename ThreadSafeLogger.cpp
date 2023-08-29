#include "ThreadSafeLogger.h"

ThreadSafeLogger::ThreadSafeLogger(const std::string& filename)
        : m_logFile(filename + "_log"), m_logStopFlag(false), m_baseTime {std::chrono::system_clock::now()}
{
    m_logThread = std::thread(&ThreadSafeLogger::logWorker, this);
}

ThreadSafeLogger::~ThreadSafeLogger() {
    m_logStopFlag = true;
    m_logCondition.notify_one();
    m_logThread.join();
    m_logFile.close();
}

std::string ThreadSafeLogger::getTimeDiff() {
    auto duration = std::chrono::system_clock::now() - m_baseTime;
    return std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) + " ms | ";
}

std::string ThreadSafeLogger::formatPID(int p_id) {
    return "P" + (p_id < 10 ? '0' + std::to_string(p_id) : std::to_string(p_id));
}

std::string ThreadSafeLogger::formatCID(const std::shared_ptr<Chopstick>& chop) {
    return "C" + (chop->id < 10 ? '0' + std::to_string(chop->id) : std::to_string(chop->id));
}

void ThreadSafeLogger::logNumberOfPhilosophers(int n_phil) {
    std::lock_guard<std::mutex> lock(m_logMutex);
    m_logQueue.push("NUM_PHIL " + std::to_string(n_phil) + "\n");
    m_logCondition.notify_one();
}

void ThreadSafeLogger::logSeatAtTable(int p_id) {
    logLineWithTS(formatPID(p_id) + " SEATS XX");
}

void ThreadSafeLogger::logLeavesTable(int p_id) {
    logLineWithTS(formatPID(p_id) + " LEAVES XX");
}

void ThreadSafeLogger::logStartEating(int p_id) {
    logLineWithTS(formatPID(p_id) + " STARTS_EATING XX");
}

void ThreadSafeLogger::logFinishEating(int p_id) {
    logLineWithTS(formatPID(p_id) + " STOPS_EATING XX");
}

void ThreadSafeLogger::logStartThinking(int p_id) {
    logLineWithTS(formatPID(p_id) + " STARTS_THINKING XX");
}

void ThreadSafeLogger::logFinishThinking(int p_id) {
    logLineWithTS(formatPID(p_id) + " STOPS_THINKING XX");
}

void ThreadSafeLogger::logPickupChopstick(int p_id, const std::shared_ptr<Chopstick> &chop, bool success) {
    if(success){
        logLineWithTS(formatPID(p_id) + " PICKS " + formatCID(chop));
    } else {
        logLineWithTS(formatPID(p_id) + " FAILS_PICKS " + formatCID(chop));
    }
}

void ThreadSafeLogger::logDropChopstick(int p_id, const std::shared_ptr<Chopstick> &chop) {
    logLineWithTS(formatPID(p_id) + " DROPS " + formatCID(chop));
}

void ThreadSafeLogger::logLineWithTS(const std::string& s) {
    std::lock_guard<std::mutex> lock(m_logMutex);
    m_logQueue.push(getTimeDiff() + s + "\n");
    m_logCondition.notify_one();
}

void ThreadSafeLogger::logWorker() {
    while (!m_logStopFlag) {
        std::unique_lock<std::mutex> lock(m_logMutex);
        m_logCondition.wait(lock, [this]() { return !m_logQueue.empty() || m_logStopFlag; });

        while (!m_logQueue.empty()) {
            m_logFile << m_logQueue.front() << std::endl;
            m_logQueue.pop();
        }
    }
}


