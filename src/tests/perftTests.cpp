#include <string>
#include <board.h>
#include <movegen.h>
#include <cassert>
#include <chrono>

struct perftTests{
    static std::string  indexToChessSquare(int index) {
        int row = 8 - index / 8;
        int col = index % 8;

        char file = 'a' + col;
        char rank = '1' + row - 1; // Adjusted to start from '1'

        std::string chessSquare = "";
        chessSquare += file;
        chessSquare += rank;

        return chessSquare;
    }

    static int perft(std::string position, int depth,  bool legal = true){
        Board b;
        b.loadFEN(position);
        int res;
        if(legal) res = generateMoves(b, depth);
        else res = generateMovesPStoLG(b, depth);
        return res;
    }

    static int generateMoves(Board& b, int depth){
        Move moves[Movegen::MAX_LEGAL_MOVES];
        int count = Movegen::generateMoves(b, moves);
        if(depth == 1) return count;
        int res = 0;
        for(int j = 0; j < count; j++){
            b.makeMove(moves[j], depth);
            res += generateMoves(b, depth-1);
            b.undoMove(moves[j], depth);
        }
        return res;
    }

    static int generateMovesPStoLG(Board& b, int depth){
        if(depth == 0) return 1;
        Move moves[Movegen::MAX_LEGAL_MOVES];
        int count = Movegen::generateMoves(b, moves, false);
        int res = 0;
        for(int j = 0; j < count; j++){
            if(moves[j].moveType == Move::CASTLING){
                UPDATE_BOARD_STATE(b, b.whoPlay);
                int kingPos = bit_ops::bitScanForward(friendlyBits[Board::KING]);
                VALIDATE_KING_CHECKS(kingPos, b, moves, j, enemyBits);
            }
            b.makeMove(moves[j], depth);
            // we need updated pieces.
            // !! changed move !! (whoplay).
            UPDATE_BOARD_STATE(b, !b.whoPlay);
            int kingPos = bit_ops::bitScanForward(friendlyBits[Board::KING]);
            bool valid = Movegen::validateKingCheck(kingPos, !b.whoPlay, enemyBits);
            if(valid) res += generateMovesPStoLG(b, depth-1);
            b.undoMove(moves[j], depth);
        }
        return res;
    }

    static bool isSquare(std::string pos, int from, int to){
        return pos == (indexToChessSquare(from)+ indexToChessSquare(to));
    }

    static void runTests(bool legalGen){
        // r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/1PN2Q1p/P1PBBPPP/R3K2R b KQkq - 0 7
        runCase("8/8/8/1PpK4/5p2/4k3/8/8 b - - 0 24", 1, 7, legalGen);
        runCase("8/8/8/1PpK4/5p2/4k3/8/8 b - - 0 24", 9, 133225511,legalGen);
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/1PN2Q1p/P1PBBPPP/R3K2R b KQkq - 0 7", 3, 81066,legalGen);
        runCase("r3k2r/p1ppqpb1/bn2p1p1/3PN3/1p2P1n1/5Q1p/PPPBBPPP/R2NK2R w KQkq - 2 7", 1, 48,legalGen);
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/5Q1p/PPPBBPPP/R2NK2R b KQkq - 1 6", 2, 2040,legalGen);
        runCase("r3k2r/p1ppqpb1/bn1Ppnp1/4N3/1p2P3/2N2Q2/PPPBBPpP/R3K2R w KQkq - 0 6", 1, 47,legalGen);
        runCase("r3k2r/p1ppqpb1/bn1Ppnp1/4N3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 5", 2, 1991,legalGen);
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 1, 48,legalGen);
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 2, 2039,legalGen);
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 3, 97862,legalGen);
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 4, 4085603,legalGen);
        runCase("r3k3/pP3pp1/1N1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2r w Qq - 0 5", 1,2,legalGen);
        runCase("r3k2r/pP3pp1/1N1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R b KQkq - 0 4", 2,913,legalGen);
        runCase("r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4", 1,34,legalGen);
        runCase("r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4", 2,751,legalGen);
        runCase("r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4", 3,23544,legalGen);
        runCase("r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4", 5, 15587335,legalGen);
        runCase("1r2k2r/1p3pp1/Pp1pr1p1/4p1P1/P3P3/3P4/2P2PP1/1R2K2R b Kk - 0 6",1,24,legalGen);
        runCase("1r2k2r/1p3pp1/Pp1pr1p1/4p1P1/4P3/P2P4/2P2PP1/1R2K2R w Kk - 1 6",2,726,legalGen);
        runCase("r3k2r/1p3pp1/Pp1pr1p1/4p1P1/4P3/P2P4/2P2PP1/1R2K2R b Kkq - 0 5",3,20168,legalGen);
        runCase("8/2p5/3p4/1P5r/KR3p2/2k5/4P1P1/8 w - - 2 2",5,982226,legalGen);
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 4,43238,legalGen);
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 1,14,legalGen);
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 2,191,legalGen);
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 3,2812,legalGen);
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 5,674624,legalGen);
        runCase("rnQ1qk1r/pp3ppp/2p5/8/1bB5/8/PPP1NKPP/RNBQ3R w - - 1 10", 1,56,legalGen);
        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NKPP/RNBQ3R b - - 0 9", 2,1396,legalGen);
        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/2P5/PP2NnPP/RNBQK2R b KQ - 0 9",2,1454,legalGen);
        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NnPP/RNBQK2R w KQ - 1 9",1,8,legalGen);
        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NnPP/RNBQK2R w KQ - 1 9",2,249,legalGen);
        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NnPP/RNBQK2R w KQ - 1 9",3,10709,legalGen);
        runCase("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",2,1486,legalGen);
        runCase("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",3,62379,legalGen);
        runCase("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",4,2103487,legalGen);
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",2,400,legalGen);
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",3,8902,legalGen);
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",4,197281,legalGen);
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5, 4865609,legalGen);
        runCase("2b1b3/1r1P4/3K3p/1p6/2p5/6k1/1P3p2/4B3 w - - 0 42", 5, 5617302,legalGen);
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 6, 11030083,legalGen);
        runCase("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 4, 3894594,legalGen);
        runCase("r3k1nr/p2pp1pp/b1n1P1P1/1BK1Pp1q/8/8/2PP1PPP/6N1 w kq - 0 1", 4, 497787,legalGen);
        runCase("3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1", 6, 1134888,legalGen);
        runCase("8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1", 6, 1440467,legalGen);
        runCase("5k2/8/8/8/8/8/8/4K2R w K - 0 1", 6, 661072,legalGen);
        runCase("3k4/8/8/8/8/8/8/R3K3 w Q - 0 1", 7, 15594314,legalGen);
        runCase("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1", 4, 1274206,legalGen);
        runCase("r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1", 5, 58773923,legalGen);
        runCase("2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1", 6, 3821001,legalGen);
        runCase("8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1", 5, 1004658,legalGen);
        runCase("4k3/1P6/8/8/8/8/K7/8 w - - 0 1", 6, 217342,legalGen);
        runCase("8/P1k5/K7/8/8/8/8/8 w - - 0 1", 6, 92683,legalGen);
        runCase("K1k5/8/P7/8/8/8/8/8 w - - 0 1", 10, 5966690,legalGen);
        runCase("8/k1P5/8/1K6/8/8/8/8 w - - 0 1", 7, 567584,legalGen);
        runCase("8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1", 6, 3114998,legalGen);
        runCase("r1bq2r1/1pppkppp/1b3n2/pP1PP3/2n5/2P5/P3QPPP/RNB1K2R w KQ a6 0 12", 5, 42761834,legalGen);
        runCase("r3k2r/pppqbppp/3p1n1B/1N2p3/1nB1P3/3P3b/PPPQNPPP/R3K2R w KQkq - 11 10", 4, 3050662,legalGen);
        runCase("4k2r/1pp1n2p/6N1/1K1P2r1/4P3/P5P1/1Pp4P/R7 w k - 0 6", 5, 10574719,legalGen);
        runCase("1Bb3BN/R2Pk2r/1Q5B/4q2R/2bN4/4Q1BK/1p6/1bq1R1rb w - - 0 1", 4, 6871272,legalGen);
        runCase("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 6, 71179139,legalGen);
        runCase("8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1", 6, 28859283,legalGen);
        runCase("8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1", 9, 7618365,legalGen);
        runCase("3r4/2p1p3/8/1P1P1P2/3K4/5k2/8/8 b - - 0 1", 4, 28181,legalGen);
        runCase("8/1p4p1/8/q1PK1P1r/3p1k2/8/4P3/4Q3 b - - 0 1", 5, 6323457,legalGen);
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",5,4865609,legalGen);
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",6,119060324,legalGen);
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 6,11030083,legalGen);
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 7,178633661,legalGen);
        runCase("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",5, 164075551,legalGen);
        runCase("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 5, 89941194,legalGen);
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 5, 193690690,legalGen);
    }

    static void runCase(std::string FEN, int depth, int expected = -1, bool legal= true){
        auto start = std::chrono::high_resolution_clock::now();

        int res = perft(FEN, depth, legal);
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