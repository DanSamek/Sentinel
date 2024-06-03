cmake_minimum_required(VERSION 3.27)
project(Sentinel)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_executable(Sentinel
        src/main.cpp
        src/bitboard.cpp
        src/movegen.h
        src/movegen.cpp)
