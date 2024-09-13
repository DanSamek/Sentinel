cmake_minimum_required(VERSION 3.25)
project(Sentinel)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_FIND_LIBRARY_SUFFIXES .a .lib)
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -lpthread")


include_directories(src/tests)
include_directories(src)

add_executable(Sentinel
        src/bit_ops.h
        src/main.cpp
        src/movegen.h
        src/movegen.cpp
        src/magics.h
        src/magics.cpp
        src/board.h
        src/board.cpp
        src/tests/BoardTests.cpp
        src/move.h
        src/move.cpp
        src/tests/PerftTests.cpp
        src/state.h
        src/uci.h
        src/uci.cpp
        src/search.h
        src/search.cpp
        src/tests/ZobristTests.cpp
        src/pst.h
        src/movepick.h
        src/movepick.cpp
        src/tt.h
        src/tt.cpp
        src/timer.h
        src/timer.cpp
        src/evaluation.cpp
        src/evaluation.cpp
        src/tests/nullMoveTests.cpp
        src/see.cpp
        src/tests/SEETests.cpp
        src/timemanager.cpp
)
