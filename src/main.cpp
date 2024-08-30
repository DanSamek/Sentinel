#include <movegen.h>
#include <tests/PerftTests.cpp>
#include <tests/BoardTests.cpp>
#include <tests/ZobristTests.cpp>
#include <tests/nullMoveTests.cpp>
#include <tests/SEETests.cpp>

#include <chrono>
#include <uci.h>

#define RUN_TESTS false

int main(){
    if(RUN_TESTS){
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

        // Convert duration to microseconds for better readability
        double microseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        std::cout << "Elapsed time: " << microseconds << " ms" << std::endl;

    }else{
        UCI::loop();
    }

    return 0;
}