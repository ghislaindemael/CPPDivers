#include <iostream>
#include <utility>
#include <vector>
#include <semaphore>
#include <thread>

using bin_sem = std::binary_semaphore;

struct Chopstick {
    int id;
    bin_sem sem;
    Chopstick(int i_id) : id { i_id }, sem(bin_sem(1)){};
};

class Philosopher {
    //shared_ptr<Logger> logger
    int id;
    bool isSeated {true };
    int timeToEat;
    int timeToThink;
    std::shared_ptr<Chopstick> left_chop;
    std::shared_ptr<Chopstick> right_chop;
    std::thread mainThread;
public:
    Philosopher(int i_id, int i_tteat, int i_ttthink, std::shared_ptr<Chopstick> i_lchop, std::shared_ptr<Chopstick> i_rchop)
        : id { i_id }, timeToEat { i_tteat }, timeToThink { i_ttthink }, left_chop { std::move(i_lchop) }, right_chop { std::move(i_rchop) },
            mainThread { std::thread(&Philosopher::dine, this) } {
                std::cout << "P" << id << " takes a seat at the table.\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
        }

    void dine(){
        while (isSeated){
            if(pickupChopstick(left_chop) && pickupChopstick(right_chop)){

                std::cout << "P" << id <<" starts to eat.\n";
                std::this_thread::sleep_for (std::chrono::seconds(timeToEat));
                std::cout << "P" << id <<" finished eating.\n";
                releaseChopsticks();

                std::cout << "P" << id << " starts thinking.\n";
                std::this_thread::sleep_for (std::chrono::seconds(timeToThink));
                std::cout << "P" << id << " stops thinking.\n";
            } else {
                releaseChopsticks();
                std::cout << "P" << id << " starts thinking.\n";
                std::this_thread::sleep_for (std::chrono::seconds(timeToThink));
                std::cout << "P" << id << " stops thinking.\n";
            }
        }
        releaseChopsticks();
        std::cout << "P" << id << " leaves the table.\n";
    }
    void leaveTable(){
        isSeated = false;
    }
    void joinThread(){
        mainThread.join();
    }
    void releaseChopsticks(){
        left_chop->sem.release();
        right_chop->sem.release();
    }
    bool pickupChopstick(std::shared_ptr<Chopstick> chopstick){
        if(chopstick->sem.try_acquire()){
            std::cout << "P" << id << " picked up chopstick " << chopstick->id << ".\n";
            return true;
        } else {
            std::cout << "P" << id << " failed to pick up chopstick " << chopstick->id << ".\n";
            return false;
        }
    }
};


int dinner(int numPhilo, int timeToEat){

    std::vector<std::shared_ptr<Chopstick>> chopsticks;
    for (int i = 0; i < numPhilo; ++i) {
        chopsticks.push_back(std::make_shared<Chopstick>(i));
    }

    std::vector<std::unique_ptr<Philosopher>> philosophers;
    for (int i = 0; i < numPhilo; ++i) {
        philosophers.push_back(std::make_unique<Philosopher>(i, timeToEat, 3, chopsticks[i], chopsticks[(i + 1) % numPhilo]));
    }

    std::this_thread::sleep_for (std::chrono::seconds(15));
    for (int i = 0; i < numPhilo; ++i) {
        philosophers[i]->leaveTable();
    }

    for (int i = 0; i < numPhilo; ++i) {
        philosophers[i]->joinThread();
    }

    return 0;
}