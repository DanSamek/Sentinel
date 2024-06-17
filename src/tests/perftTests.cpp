#include <string>
#include <board.h>
#include <movegen.h>
#include <cassert>

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
            b.makeMove(moves[j], depth);
            if(DEBUG && depth == 3 ){
                std::cout << indexToChessSquare(moves[j].fromSq) <<  indexToChessSquare(moves[j].toSq) << "-";
                int tmp = generateMoves(b, depth-1);
                std::cout << tmp << std::endl;
                res += tmp;
            }
            else{
                res+= generateMoves(b, depth-1);
            }

            b.undoMove(moves[j], depth);

        }
        return res;
    }


    static void runTests(){

        assert(perft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 2, true) == 1486);
        std::cout << perft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ", 3, true) << std::endl;
        assert(perft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ", 3) == 62379);
        assert(perft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ", 4) == 2103487);


        assert(perft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2) == 400);
        assert(perft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3) == 8902);
        assert(perft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4) == 197281);
        assert(perft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5) == 4865609);
        assert(perft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6) == 119060324);
    }
};