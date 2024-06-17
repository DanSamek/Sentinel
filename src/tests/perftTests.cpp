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

    static int perft(std::string position, int depth, bool debug = false){
        Board b;
        b.loadFEN(position);
        DEBUG = debug;
        int res = generateMoves(b, depth);
        return res;
    }//7+11 + 1  + 2 + 1 + 2 + 6

    static inline bool DEBUG = false;

    static int generateMoves(Board& b, int depth){
        if(depth == 0) return 1;
        Move moves[Movegen::MAX_LEGAL_MOVES];
        int count = Movegen::generateMoves(b, moves);
        int res = 0;
        for(int j = 0; j < count; j++){
            /*
            if(isSquare("e2e4", moves[j].fromSq, moves[j].toSq) && moves[j].moveType == Move::DOUBLE_PAWN_UP){
                std::cout << std::endl;
            }
             */
            /*
            b.makeMove(moves[j], depth);

            if(DEBUG && depth >= 2){
                int tmp = generateMoves(b, depth-1);
                for(int j = 0; j < depth; j++) std::cout << "    ";
                std::cout << indexToChessSquare(moves[j].fromSq) <<  indexToChessSquare(moves[j].toSq) << "-" << tmp << std::endl;
                res += tmp;
            }
            else{
            }*/
            b.makeMove(moves[j], depth);
            res+= generateMoves(b, depth-1);
            b.undoMove(moves[j], depth);

        }
        return res;
    }

    static bool isSquare(std::string pos, int from, int to){
        return pos == (indexToChessSquare(from)+ indexToChessSquare(to));
    }

    static void runTests(){
        // rnQ1qk1r/pp3ppp/2p5/8/1bB5/8/PPP1NKPP/RNBQ3R w - - 1 10

        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 4,43238, true);
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 1,14);
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 2,191);
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 3,2812);
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 5,674624);
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 6,11030083);
        runCase("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 7,178633661);

        runCase("rnQ1qk1r/pp3ppp/2p5/8/1bB5/8/PPP1NKPP/RNBQ3R w - - 1 10", 1,56);
        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NKPP/RNBQ3R b - - 0 9", 2,1396);


        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/2P5/PP2NnPP/RNBQK2R b KQ - 0 9",2,1454);

        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NnPP/RNBQK2R w KQ - 1 9",1,8);
        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NnPP/RNBQK2R w KQ - 1 9",2,249);
        runCase("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NnPP/RNBQK2R w KQ - 1 9",3,10709);

        runCase("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",2,1486);
        runCase("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",3,62379);
        runCase("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",4,2103487);


        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",2,400);
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",3,8902);
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",4,197281);
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",5,4865609);
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",6,119060324);

        runCase("8/2p5/3p4/1P5r/KR3p2/2k5/4P1P1/8 w - - 2 2",5,982226);

        runCase("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",5, 164075551);

    }

    static void runCase(std::string FEN, int depth, int expected = -1, bool debug = false){
        auto start = std::chrono::high_resolution_clock::now();

        int res = perft(FEN, depth, debug);
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