cmake_minimum_required(VERSION 3.25)
project(C__Divers)

set(CMAKE_CXX_STANDARD 20)

add_executable(C__Divers main.cpp philosophers.cpp philosophers.h dinner_checker.cpp dinner_checker.h GLOBALS.h GLOBALS.cpp)
