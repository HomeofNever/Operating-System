cmake_minimum_required(VERSION 3.18)
project(hw3 C)

set(CMAKE_C_STANDARD 99)

add_executable(hw3 hw3-main.c hw3.c)
add_definitions(-DNO_PARALLEL)

find_package(Threads REQUIRED)
target_link_libraries(hw3 pthread)

