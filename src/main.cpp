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
        /*
        NeuralNetwork n;
        n.load();
        Board b;
        b.loadFEN("r1bqkbnr/ppp1pppp/2n5/3p4/3P4/1BN2N2/PPP1PPPP/R1BQ1RK1 w kq - 0 1");
        n.eval(b);
        b.loadFEN("r2qkb1r/ppp1pppp/2n2n2/3p1b2/3P4/1BN2N2/PPP1PPPP/R1BQ1RK1 w kq - 0 1");
        n.eval(b);

        b.loadFEN("rnbqkbnr/pppppppp/8/3P4/4P3/1BN2N2/PPP1QPPP/R1B2RK1 w kq - 0 1");
        n.eval(b);

        b.loadFEN("r1bqkbnr/pppp1ppp/2n5/4p3/4P3/3P4/PPP2PPP/RNBQKBNR w KQkq - 0 1");
        n.eval(b);

        b.loadFEN("r1bqkbnr/pppp1ppp/2n5/4p3/4P3/3P4/PPP2PPP/RNBQKBNR b KQkq - 0 1");
        n.eval(b);

        191
        79
        469
        -55
        101
        return 0;
         */
        Datagen(24,7'500,5'000'000).run();
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