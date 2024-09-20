#include <movegen.h>
#include <tests/PerftTests.cpp>
#include <tests/BoardTests.cpp>
#include <tests/ZobristTests.cpp>
#include <tests/nullMoveTests.cpp>
#include <tests/SEETests.cpp>
#include <tests/NNUETests.cpp>
#include "nnue/datagen.h"
#include <chrono>
#include <uci.h>
#include "development.h"
#include "nnue/neuralnetwork.h"

int main(){

    if(RUN_DATAGEN){
        Board::initPawnEvalBBS();
        Movegen::init();
        Zobrist::init();
        PST::init();
        NeuralNetwork n;
        n.test();

      /*  191
        79
        469
        -55
        101*/

//        Datagen(24,7'500,5'000'000).run();
    }
    else if(RUN_TESTS){
        Board::initPawnEvalBBS();
        Movegen::init();
        Zobrist::init();
        PST::init();
        NNUETests::testNNUE();
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