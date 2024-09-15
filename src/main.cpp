#include <movegen.h>
#include <tests/PerftTests.cpp>
#include <tests/BoardTests.cpp>
#include <tests/ZobristTests.cpp>
#include <tests/nullMoveTests.cpp>
#include <tests/SEETests.cpp>
#include "nnue/datagen.h"
#include <chrono>
#include <uci.h>

#define RUN_TESTS false
#define RUN_DATAGEN true

int main(){
    if(RUN_DATAGEN){
        // TODO.
        Datagen(8).run();
    }
    else if(RUN_TESTS){
        Board::initPawnEvalBBS();
        Movegen::init();
        Zobrist::init();
        PST::init();
        ZobristTests::runTests();
        SEETests::run();
        BoardTests::testBoard();
        NullMoveTests::runTests();
        auto start = std::chrono::high_resolution_clock::now();
        PerftTests::runTests();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        double microseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        std::cout << "Elapsed time: " << microseconds << " ms" << std::endl;

    }else{
        UCI::loop();
    }

    return 0;
}