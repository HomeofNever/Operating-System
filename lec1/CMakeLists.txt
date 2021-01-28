cmake_minimum_required(VERSION 3.19)
project(lec1 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(lec1 extract.c)
