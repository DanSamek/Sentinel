cmake_minimum_required(VERSION 3.25)
project(Sentinel)

if(NOT DEFINED EXE)
    set(EXE "Sentinel" CACHE STRING "Executable name")
    set(EXE_DEFINED OFF)
else()
    set(EXE_DEFINED ON)
endif()

option(ENABLE_AVX "Enable AVX" OFF)
option(ENABLE_AVX_512 "Enable AVX512" OFF)
option(ENABLE_SSE, "Enable SSE" OFF)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a .lib)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -lpthread")
endif ()


if(EXE_DEFINED)
    message(STATUS "EXE defined as ${EXE}, enabling Release build with AVX")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)

    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-mavx" COMPILER_SUPPORTS_AVX)
    if(COMPILER_SUPPORTS_AVX)
        add_definitions(-DENABLE_AVX)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mavx")
    endif()

    check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)
    if(COMPILER_SUPPORTS_AVX2)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mavx2")
    endif()
endif()


if(ENABLE_AVX)
    include(CheckCXXCompilerFlag)

    check_cxx_compiler_flag("-mavx" COMPILER_SUPPORTS_AVX)
    check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)

    if(COMPILER_SUPPORTS_AVX)
        add_definitions(-DENABLE_AVX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")
    endif()

    if(COMPILER_SUPPORTS_AVX2)
        add_definitions(-DENABLE_AVX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
    endif()
endif()

if(ENABLE_AVX_512)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-mavx512f" COMPILER_SUPPORTS_AVX_512)
    if(COMPILER_SUPPORTS_AVX_512)
        add_definitions(-DENABLE_AVX_512)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512f")
    endif()
endif ()

if(ENABLE_SSE)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-msse" COMPILER_SUPPORTS_SSE)
    if(COMPILER_SUPPORTS_SSE)
        add_definitions(-DENABLE_SSE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse4.1")
    endif()
endif ()


include_directories(src/tests)
include_directories(src)

add_executable(${EXE}
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
        src/tests/NullMoveTests.cpp
        src/see.cpp
        src/tests/SEETests.cpp
        src/timemanager.cpp
        src/nnue/datagen.h
        src/nnue/datagen.cpp
        src/nnue/neuralnetwork.h
        src/nnue/const.h
        src/nnue/neuralnetwork.cpp
        src/nnue/neuralnetwork.cpp
        src/nnue/accumulator.cpp
        src/nnue/nnue.cpp
        src/boardEnums.h
        src/tests/NNUETests.cpp
        src/tunable.h
        src/tunable.cpp
)
