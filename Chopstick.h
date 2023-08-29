#ifndef CHOPSTICK_H
#define CHOPSTICK_H

#include <semaphore>

struct Chopstick {
public:
    int id;
    std::binary_semaphore sem;

    explicit Chopstick(int i_id);
    ~Chopstick();
};

#endif // CHOPSTICK_H
