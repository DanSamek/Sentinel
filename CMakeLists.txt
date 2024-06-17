cmake_minimum_required(VERSION 3.27)
project(Sentinel)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

include_directories(src/tests)
include_directories(src)

add_executable(Sentinel
        src/main.cpp
        src/bitboard.cpp
        src/movegen.h
        src/movegen.cpp
        src/magics.h
        src/magics.cpp
        src/board.h
        src/board.cpp
        src/tests/boardTests.cpp
        src/move.h
        src/move.cpp
        src/bit_ops.h
        src/bit_ops.cpp
        src/State.h
        src/tests/perftTests.cpp
)
