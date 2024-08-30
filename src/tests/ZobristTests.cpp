#include <cassert>
#include <board.h>
#include <movegen.h>
#include <zobrist.h>
struct ZobristTests{

    static void runTests(){
        Zobrist::init();
        Board b;
        b.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        auto initHash = b.zobristKey;
        Move moves[Movegen::MAX_LEGAL_MOVES];
        Movegen(b, moves, false).generateMoves();

        auto zobrist = b.zobristKey;
        assert(b.makeMove(moves[0]) == true);
        assert(zobrist != b.zobristKey);
        b.undoMove(moves[0]);
        assert(zobrist == b.zobristKey);
        assert(initHash == zobrist);
        assert(initHash == b.zobristKey);


        // 2 MOVES.
        assert(b.makeMove(moves[0]) == true);
        auto zobrist1 = b.zobristKey;

        Move moves2[Movegen::MAX_LEGAL_MOVES];
        Movegen(b, moves2, false).generateMoves();

        assert(b.makeMove(moves2[0]) == true);
        auto zobrist2 = b.zobristKey; // uniq
        b.undoMove(moves2[0]);

        assert(b.zobristKey == zobrist1);

        b.undoMove(moves[0]);
        assert(b.zobristKey == zobrist);

        assert(zobrist2 != zobrist && zobrist2 != zobrist1);

        // try simple position with different moves.
        b.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        // load init pos
        assert(b.zobristKey == initHash);

        Move Moves[Movegen::MAX_LEGAL_MOVES];

        int cnt = Movegen(b, Moves, false).generateMoves().first;

        // try play moves.
        // from = 52, double pawn up
        auto move = trySearchMove(Moves, cnt, 52, 36);
        b.makeMove(move);
        b.printBoard();

        // from = 1, to 18
        cnt = Movegen(b, Moves, false).generateMoves().first;
        move = trySearchMove(Moves, cnt, 1, 18);
        b.makeMove(move);
        b.printBoard();

        // from = 62 to 45
        cnt = Movegen(b, Moves, false).generateMoves().first;
        move = trySearchMove(Moves, cnt, 62, 45);
        b.makeMove(move);
        b.printBoard();

        // from = 11 to 27
        cnt = Movegen(b, Moves, false).generateMoves().first;
        move = trySearchMove(Moves, cnt, 11, 27);
        b.makeMove(move);
        b.printBoard();

        auto patternHash = b.zobristKey;

        // reset.
        b.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        assert(b.zobristKey == initHash);

        // from = 62 to 45
        cnt = Movegen(b, Moves, false).generateMoves().first;
        move = trySearchMove(Moves, cnt, 62, 45);
        b.makeMove(move);


        // from = 11 to 27
        cnt = Movegen(b, Moves, false).generateMoves().first;
        move = trySearchMove(Moves, cnt, 11, 27);
        b.makeMove(move);
        b.printBoard();

        // from = 52, double pawn up
        cnt = Movegen(b, Moves, false).generateMoves().first;
        move = trySearchMove(Moves, cnt, 52, 36);
        b.makeMove(move);
        b.printBoard();

        // from = 1, to 18
        cnt = Movegen(b, Moves, false).generateMoves().first;
        move = trySearchMove(Moves, cnt, 1, 18);
        b.makeMove(move);
        b.printBoard();

        std::cout << b.zobristKey << " != " << patternHash << std::endl;
        assert(b.zobristKey == patternHash);


        // end-game shuffling with pieces.
        b.loadFEN("7r/8/8/4k3/8/4K3/8/R7 w - - 0 1");
        b.printBoard();
        initHash = b.zobristKey;

        cnt = Movegen(b, Moves, false).generateMoves().first;
        b.makeMove(trySearchMove(Moves, cnt, 56,0)); // UP w rook

        cnt = Movegen(b, Moves, false).generateMoves().first;
        b.makeMove(trySearchMove(Moves, cnt, 7,63)); // DOWN b rook
        assert(initHash != b.zobristKey);
        b.printBoard();

        cnt = Movegen(b, Moves, false).generateMoves().first;
        b.makeMove(trySearchMove(Moves, cnt, 0,56)); // DOWN w rook

        cnt = Movegen(b, Moves, false).generateMoves().first;
        b.makeMove(trySearchMove(Moves, cnt, 63,7)); // UP b rook

        b.printBoard();

        assert(initHash == b.zobristKey);
        assert(!b.isDraw());
        // try 3-fold repetition.

        cnt = Movegen(b, Moves, false).generateMoves().first;
        b.makeMove(trySearchMove(Moves, cnt, 56,0)); // UP w rook

        cnt = Movegen(b, Moves, false).generateMoves().first;
        b.makeMove(trySearchMove(Moves, cnt, 7,63)); // DOWN b rook
        assert(initHash != b.zobristKey);
        b.printBoard();

        cnt = Movegen(b, Moves, false).generateMoves().first;
        b.makeMove(trySearchMove(Moves, cnt, 0,56)); // DOWN w rook

        cnt = Movegen(b, Moves, false).generateMoves().first;
        b.makeMove(trySearchMove(Moves, cnt, 63,7)); // UP b rook

        b.printBoard();
        assert(initHash == b.zobristKey);
        assert(b.isDraw());


        b.loadFEN("7r/8/8/4k3/8/4K3/8/R7 w - - 0 1");
        auto whiteHash = b.zobristKey;

        b.loadFEN("7r/8/8/4k3/8/4K3/8/R7 b - - 0 1");
        auto blackHash = b.zobristKey;

        assert(whiteHash != blackHash);

    }

    static Move trySearchMove(Move* moves, int cnt, int from, int to){
        for(int j = 0; j < cnt; j++){
            if(moves[j].fromSq == from && moves[j].toSq == to) return moves[j];
        }
        assert(false);
    }
};