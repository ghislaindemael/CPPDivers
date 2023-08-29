cmake_minimum_required(VERSION 3.25)
project(C__Divers)

set(CMAKE_CXX_STANDARD 20)

add_executable(C__Divers main.cpp Dinner.cpp Dinner.h ThreadSafeLogger.cpp ThreadSafeLogger.h Chopstick.cpp Chopstick.h Philosopher.cpp Philosopher.h Checker.cpp Checker.h Arguments.cpp Arguments.h)
