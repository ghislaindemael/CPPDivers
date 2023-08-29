#include "Chopstick.h"

Chopstick::Chopstick(int i_id) : id(i_id), sem(std::binary_semaphore(1)){}

Chopstick::~Chopstick() {}

