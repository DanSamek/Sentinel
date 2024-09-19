#include <movegen.h>
#include <tests/PerftTests.cpp>
#include <tests/BoardTests.cpp>
#include <tests/ZobristTests.cpp>
#include <tests/nullMoveTests.cpp>
#include <tests/SEETests.cpp>
#include "nnue/datagen.h"
#include <chrono>
#include <uci.h>
#include "development.h"
#include "nnue/neuralnetwork.h"

int main(){

    if(RUN_DATAGEN){
        NeuralNetwork n;
        n.load();
        Board b;
        b.loadFEN("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");
        n.eval(b);
        b.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNB1KBNR w KQkq - 0 1");
        n.eval(b);

        return 0;
        Datagen(24,7'500,2'000'000).run();
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