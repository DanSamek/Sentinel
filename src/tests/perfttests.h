#ifndef SENTINEL_PERFTTESTS_H
#define SENTINEL_PERFTTESTS_H

#include <string>
#include <board.h>
#include <movegen.h>
#include <cassert>
#include <chrono>
#include "zobrist.h"
using namespace Sentinel;

struct PerftTests{


    static int perft(std::string position, int depth){
        Sentinel::Board b;
        b.loadFEN(position);
        int res;
        res = generateMoves(b, depth);
        return res;
    }

    static int generateMoves(Sentinel::Board& b, int depth){
        if(depth == 0) return 1;
        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto result = Movegen(b, moves).generateMoves<false>();
        int res = 0;
        for(int j = 0; j < result.first; j++){
            auto key = b.zobristKey;
            if(!b.makeMove(moves[j])) continue;
            res += generateMoves(b, depth-1);
            b.undoMove();
            assert(b.zobristKey == key);

        }
        return res;
    }

    static void printMove(const Move& move){
        std::cout << indexToSquare(move.fromSq) << indexToSquare(move.toSq) << "-1" << std::endl;
    }

    static std::string indexToSquare(int index) {
        char file = 'a' + (index % 8);
        char rank = '8' - (index / 8);

        return std::string(1, file) + rank;
    }

    static void run(){

        // r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/1PN2Q1p/P1PBBPPP/R3K2R b KQkq - 0 7
        runCase("r3k3/pP3pp1/1N1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2r w Qq - 0 5", 1,2 );
        runCase("r3k2r/p1ppqpb1/bn2p1p1/3PN3/1p2P1n1/5Q1p/PPPBBPPP/R2NK2R w KQkq - 2 7", 1, 48 );
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/5Q1p/PPPBBPPP/R2NK2R b KQkq - 1 6", 2, 2040 );
        runCase("r3k2r/p1ppqpb1/bn1Ppnp1/4N3/1p2P3/2N2Q2/PPPBBPpP/R3K2R w KQkq - 0 6", 1, 47 );
        runCase("8/8/8/1PpK4/5p2/4k3/8/8 b - - 0 24", 1, 7);
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 1, 48 );
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/1PN2Q1p/P1PBBPPP/R3K2R b KQkq - 0 7", 3, 81066 );
        runCase("r3k2r/p1ppqpb1/bn1Ppnp1/4N3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 5", 2, 1991 );
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 2, 2039 );
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 3, 97862 );
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 4, 4085603 );
        runCase("r3k2r/pP3pp1/1N1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R b KQkq - 0 4", 2,913 );
        runCase("r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4", 1,34 );
        runCase("r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4", 2,751 );
        runCase("r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4", 3,23544 );
        runCase("r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4", 5, 15587335 );
        runCase("1r2k2r/1p3pp1/Pp1pr1p1/4p1P1/P3P3/3P4/2P2PP1/1R2K2R b Kk - 0 6",1,24 );
        runCase("1r2k2r/1p3pp1/Pp1pr1p1/4p1P1/4P3/P2P4/2P2PP1/1R2K2R w Kk - 1 6",2,726 );
        runCase("r3k2r/1p3pp1/Pp1pr1p1/4p1P1/4P3/P2P4/2P2PP1/1R2K2R b Kkq - 0 5",3,20168 );
        runCase("8/2p5/3p4/1P5r/KR3p2/2k5/4P1P1/8 w - - 2 2",5,982226 );
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 4,43238 );
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 1,14 );
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 2,191 );
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 3,2812 );
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 5,674624 );
        runCase("rnQ1qk1r/pp3ppp/2p5/8/1bB5/8/PPP1NKPP/RNBQ3R w - - 1 10", 1,56 );
        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NKPP/RNBQ3R b - - 0 9", 2,1396 );
        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/2P5/PP2NnPP/RNBQK2R b KQ - 0 9",2,1454 );
        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NnPP/RNBQK2R w KQ - 1 9",1,8 );
        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NnPP/RNBQK2R w KQ - 1 9",2,249 );
        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NnPP/RNBQK2R w KQ - 1 9",3,10709 );
        runCase("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",2,1486 );
        runCase("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",3,62379 );
        runCase("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",4,2103487 );
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",2,400 );
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",3,8902 );
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",4,197281 );
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5, 4865609 );
        runCase("2b1b3/1r1P4/3K3p/1p6/2p5/6k1/1P3p2/4B3 w - - 0 42", 5, 5617302 );
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 6, 11030083 );
        runCase("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 4, 3894594 );
        runCase("r3k1nr/p2pp1pp/b1n1P1P1/1BK1Pp1q/8/8/2PP1PPP/6N1 w kq - 0 1", 4, 497787 );
        runCase("3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1", 6, 1134888 );
        runCase("8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1", 6, 1440467 );
        runCase("5k2/8/8/8/8/8/8/4K2R w K - 0 1", 6, 661072 );
        runCase("3k4/8/8/8/8/8/8/R3K3 w Q - 0 1", 7, 15594314 );
        runCase("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1", 4, 1274206 );
        runCase("r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1", 5, 58773923 );
        runCase("2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1", 6, 3821001 );
        runCase("8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1", 5, 1004658 );
        runCase("4k3/1P6/8/8/8/8/K7/8 w - - 0 1", 6, 217342 );
        runCase("8/P1k5/K7/8/8/8/8/8 w - - 0 1", 6, 92683 );
        runCase("K1k5/8/P7/8/8/8/8/8 w - - 0 1", 10, 5966690 );
        runCase("8/k1P5/8/1K6/8/8/8/8 w - - 0 1", 7, 567584 );
        runCase("8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1", 6, 3114998 );
        runCase("r1bq2r1/1pppkppp/1b3n2/pP1PP3/2n5/2P5/P3QPPP/RNB1K2R w KQ a6 0 12", 5, 42761834 );
        runCase("r3k2r/pppqbppp/3p1n1B/1N2p3/1nB1P3/3P3b/PPPQNPPP/R3K2R w KQkq - 11 10", 4, 3050662 );
        runCase("4k2r/1pp1n2p/6N1/1K1P2r1/4P3/P5P1/1Pp4P/R7 w k - 0 6", 5, 10574719 );
        runCase("1Bb3BN/R2Pk2r/1Q5B/4q2R/2bN4/4Q1BK/1p6/1bq1R1rb w - - 0 1", 4, 6871272 );
        runCase("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 6, 71179139 );
        runCase("8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1", 6, 28859283 );
        runCase("8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1", 9, 7618365 );
        runCase("3r4/2p1p3/8/1P1P1P2/3K4/5k2/8/8 b - - 0 1", 4, 28181 );
        runCase("8/1p4p1/8/q1PK1P1r/3p1k2/8/4P3/4Q3 b - - 0 1", 5, 6323457 );
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",5,4865609 );
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",6,119060324 );
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 6,11030083 );
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 7,178633661 );
        runCase("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",5, 164075551 );
        runCase("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 5, 89941194 );
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 5, 193690690 );
        runCase("8/8/8/1PpK4/5p2/4k3/8/8 b - - 0 24", 9, 133225511 );
    }

    static void runCase(std::string FEN, int depth, int expected = -1){
        auto start = std::chrono::high_resolution_clock::now();

        int res = perft(FEN, depth);
        if(expected != -1){
            if(res != expected) std::cout << "ref: "<< expected<< " curr:" << res << std::endl;
            assert(res == expected);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        double ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        std::cout << FEN << ": " << ms << "ms; " << res << " nodes" << std::endl;
    }
};

#endif //SENTINEL_PERFTTESTS_H
