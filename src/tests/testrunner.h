#ifndef SENTINEL_TESTRUNNER_H
#define SENTINEL_TESTRUNNER_H

#include "zobrist.h"
#include "chrono"
#include "movegen.h"
#include "boardtests.h"
#include "nullmovetests.h"
#include "perfttests.h"
#include "seetests.h"
#include "zobristtests.h"

struct TestRunner{

    static void run(){
        Sentinel::Movegen::init();
        Sentinel::Zobrist::init();

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

#endif //SENTINEL_TESTRUNNER_H
