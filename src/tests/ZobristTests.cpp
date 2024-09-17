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
        Movegen(b, moves).generateMoves<false>();

        auto zobrist = b.zobristKey;
        assert(b.makeMove(moves[0]) == true);
        assert(zobrist != b.zobristKey);

        assert("rnbqkbnr/pppppppp/8/8/P7/8/1PPPPPPP/RNBQKBNR b KQkq a3 0 1" == b.FEN());
        b.undoMove(moves[0]);
        assert("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" == b.FEN());

        assert(zobrist == b.zobristKey);
        assert(initHash == zobrist);
        assert(initHash == b.zobristKey);


        // 2 MOVES.
        assert(b.makeMove(moves[0]) == true);
        auto zobrist1 = b.zobristKey;
        assert("rnbqkbnr/pppppppp/8/8/P7/8/1PPPPPPP/RNBQKBNR b KQkq a3 0 1" == b.FEN());

        Move moves2[Movegen::MAX_LEGAL_MOVES];
        Movegen(b, moves2).generateMoves<false>();

        assert(b.makeMove(moves2[0]) == true);
        assert("rnbqkbnr/1ppppppp/p7/8/P7/8/1PPPPPPP/RNBQKBNR w KQkq - 0 2" == b.FEN());

        auto zobrist2 = b.zobristKey; // uniq
        b.undoMove(moves2[0]);
        assert("rnbqkbnr/pppppppp/8/8/P7/8/1PPPPPPP/RNBQKBNR b KQkq a3 0 1" == b.FEN());

        assert(b.zobristKey == zobrist1);
        assert("rnbqkbnr/pppppppp/8/8/P7/8/1PPPPPPP/RNBQKBNR b KQkq a3 0 1" == b.FEN());

        b.undoMove(moves[0]);
        assert(b.zobristKey == zobrist);

        assert(zobrist2 != zobrist && zobrist2 != zobrist1);

        // try simple position with different moves.
        b.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        // load init pos
        assert(b.zobristKey == initHash);

        Move Moves[Movegen::MAX_LEGAL_MOVES];

        int cnt = Movegen(b, Moves).generateMoves<false>().first;

        // try play moves.
        // from = 52, double pawn up
        auto move = trySearchMove(Moves, cnt, 52, 36);
        b.makeMove(move);
        b.printBoard();
        assert(b.FEN() == "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");

        // from = 1, to 18
        cnt = Movegen(b, Moves).generateMoves<false>().first;
        move = trySearchMove(Moves, cnt, 1, 18);
        b.makeMove(move);
        b.printBoard();
        assert(b.FEN() == "r1bqkbnr/pppppppp/2n5/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 1 2");

        // from = 62 to 45
        cnt = Movegen(b, Moves).generateMoves<false>().first;
        move = trySearchMove(Moves, cnt, 62, 45);
        b.makeMove(move);
        b.printBoard();
        assert(b.FEN() == "r1bqkbnr/pppppppp/2n5/8/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 2 2");

        // from = 11 to 27
        cnt = Movegen(b, Moves).generateMoves<false>().first;
        move = trySearchMove(Moves, cnt, 11, 27);
        b.makeMove(move);
        b.printBoard();
        assert(b.FEN() == "r1bqkbnr/ppp1pppp/2n5/3p4/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq d6 0 3");

        auto patternHash = b.zobristKey;

        // reset.
        b.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        assert(b.zobristKey == initHash);

        // from = 62 to 45
        cnt = Movegen(b, Moves).generateMoves<false>().first;
        move = trySearchMove(Moves, cnt, 62, 45);
        b.makeMove(move);
        b.printBoard();
        assert(b.FEN() == "rnbqkbnr/pppppppp/8/8/8/5N2/PPPPPPPP/RNBQKB1R b KQkq - 1 1");

        // from = 11 to 27
        cnt = Movegen(b, Moves).generateMoves<false>().first;
        move = trySearchMove(Moves, cnt, 11, 27);
        b.makeMove(move);
        b.printBoard();
        assert(b.FEN() == "rnbqkbnr/ppp1pppp/8/3p4/8/5N2/PPPPPPPP/RNBQKB1R w KQkq d6 0 2");

        // from = 52, double pawn up
        cnt = Movegen(b, Moves).generateMoves<false>().first;
        move = trySearchMove(Moves, cnt, 52, 36);
        b.makeMove(move);
        b.printBoard();
        assert(b.FEN() == "rnbqkbnr/ppp1pppp/8/3p4/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq e3 0 2");

        // from = 1, to 18
        cnt = Movegen(b, Moves).generateMoves<false>().first;
        move = trySearchMove(Moves, cnt, 1, 18);
        b.makeMove(move);
        b.printBoard();
        assert(b.FEN() == "r1bqkbnr/ppp1pppp/2n5/3p4/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 1 3");

        assert(b.zobristKey == patternHash);


        // end-game shuffling with pieces.
        b.loadFEN("7r/8/8/4k3/8/4K3/8/R7 w - - 0 1");
        b.printBoard();
        initHash = b.zobristKey;

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves, cnt, 56,0)); // UP w rook

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves, cnt, 7,63)); // DOWN b rook
        assert(initHash != b.zobristKey);
        b.printBoard();

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves, cnt, 0,56)); // DOWN w rook

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves, cnt, 63,7)); // UP b rook

        b.printBoard();

        assert(initHash == b.zobristKey);
        assert(!b.isDraw());
        // try 3-fold repetition.

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves, cnt, 56,0)); // UP w rook

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves, cnt, 7,63)); // DOWN b rook
        assert(initHash != b.zobristKey);
        b.printBoard();

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves, cnt, 0,56)); // DOWN w rook

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves, cnt, 63,7)); // UP b rook

        b.printBoard();
        assert(initHash == b.zobristKey);
        assert(b.isDraw());


        b.loadFEN("7r/8/8/4k3/8/4K3/8/R7 w - - 0 1");
        auto whiteHash = b.zobristKey;

        b.loadFEN("7r/8/8/4k3/8/4K3/8/R7 b - - 0 1");
        auto blackHash = b.zobristKey;

        assert(whiteHash != blackHash);

        b.loadFEN("r1bqkbnr/ppp3pp/2n2p2/3pp3/4P3/5NP1/PPPP1PBP/RNBQK2R w KQkq - 0 5");
        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves,cnt, 60, 62));
        assert(b.FEN() == "r1bqkbnr/ppp3pp/2n2p2/3pp3/4P3/5NP1/PPPP1PBP/RNBQ1RK1 b kq - 1 5");

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves,cnt, 9, 17));
        assert(b.FEN() == "r1bqkbnr/p1p3pp/1pn2p2/3pp3/4P3/5NP1/PPPP1PBP/RNBQ1RK1 w kq - 0 6");

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves,cnt, 36, 27));
        assert(b.FEN() == "r1bqkbnr/p1p3pp/1pn2p2/3Pp3/8/5NP1/PPPP1PBP/RNBQ1RK1 b kq - 0 6");

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves,cnt, 2, 9));
        assert(b.FEN() == "r2qkbnr/pbp3pp/1pn2p2/3Pp3/8/5NP1/PPPP1PBP/RNBQ1RK1 w kq - 1 7");

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves,cnt, 61, 60));
        assert(b.FEN() == "r2qkbnr/pbp3pp/1pn2p2/3Pp3/8/5NP1/PPPP1PBP/RNBQR1K1 b kq - 2 7");

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves,cnt, 3, 19));
        assert(b.FEN() == "r3kbnr/pbp3pp/1pnq1p2/3Pp3/8/5NP1/PPPP1PBP/RNBQR1K1 w kq - 3 8");

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves,cnt, 57, 42));
        assert(b.FEN() == "r3kbnr/pbp3pp/1pnq1p2/3Pp3/8/2N2NP1/PPPP1PBP/R1BQR1K1 b kq - 4 8");

        cnt = Movegen(b, Moves).generateMoves<false>().first;
        b.makeMove(trySearchMove(Moves,cnt, 4, 2));
        assert(b.FEN() == "2kr1bnr/pbp3pp/1pnq1p2/3Pp3/8/2N2NP1/PPPP1PBP/R1BQR1K1 w - - 5 9");

        b.loadFEN("2k4r/pb6/1p4p1/8/1P4P1/P4PK1/7P/8 w - - 5 9");
        assert(b.FEN() == "2k4r/pb6/1p4p1/8/1P4P1/P4PK1/7P/8 w - - 5 9");

        std::cout << "Zobrist + board.FEN(): DONE" << std::endl;
    }

    static Move trySearchMove(Move* moves, int cnt, int from, int to){
        for(int j = 0; j < cnt; j++){
            if(moves[j].fromSq == from && moves[j].toSq == to) return moves[j];
        }
        assert(false);
    }
};