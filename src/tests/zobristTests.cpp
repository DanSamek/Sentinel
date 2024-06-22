#include <cassert>
#include <board.h>
#include <movegen.h>
#include <zobrist.h>
struct zobristTests{

    static void runTests(){
        Zobrist::init();
        Board b;
        b.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        auto initHash = b.zobristKey;
        Move moves[Movegen::MAX_LEGAL_MOVES];
        Movegen::generateMoves(b, moves);

        auto zobrist = b.zobristKey;
        b.makeMove(moves[0]);
        assert(zobrist != b.zobristKey);
        b.undoMove(moves[0]);
        assert(zobrist == b.zobristKey);
        assert(initHash == zobrist);
        assert(initHash == b.zobristKey);


        // 2 MOVES.
        b.makeMove(moves[0]);
        auto zobrist1 = b.zobristKey;

        Move moves2[Movegen::MAX_LEGAL_MOVES];
        Movegen::generateMoves(b, moves2);

        b.makeMove(moves2[0]);
        auto zobrist2 = b.zobristKey; // uniq
        b.undoMove(moves2[0]);

        assert(b.zobristKey == zobrist1);

        b.undoMove(moves[0]);
        assert(b.zobristKey == zobrist);

        assert(zobrist2 != zobrist && zobrist2 != zobrist1);
        // other tests are done in perft tests.
    }
};