#include "zobrist.h"
#include "chrono"
#include "movegen.h"
#include "./tests/NNUETests.cpp"
#include "./tests/ZobristTests.cpp"
#include "./tests/SEETests.cpp"
#include "./tests/BoardTests.cpp"
#include "./tests/NullMoveTests.cpp"
#include "./tests/PerftTests.cpp"

struct TestRunner{

    static void run(){
        Board::initPawnEvalBBS();
        Movegen::init();
        Zobrist::init();
        PST::init();

        NNUETests::run();
        ZobristTests::run();
        SEETests::run();
        BoardTests::run();
        NullMoveTests::run();
        auto start = std::chrono::high_resolution_clock::now();
        PerftTests::run();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        double microseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        std::cout << "Elapsed time: " << microseconds << " ms" << std::endl;
    }
};