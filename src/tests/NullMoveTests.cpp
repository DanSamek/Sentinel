#include <zobrist.h>
#include <movegen.h>
#include <chrono>

struct NullMoveTests{

    static int perft(std::string position, int depth){
        Board b;
        b.loadFEN(position);
        int res;
        res = generateMoves(b, depth);
        return res;
    }

    static int generateMoves(Board& b, int depth){
        if(depth == 0) return 1;
        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto result = Movegen(b, moves).generateMoves<false>();
        int res = 0;
        for(int j = 0; j < result.first; j++){
            auto key = b.zobristKey;
            if(!b.makeMove(moves[j])) continue;
            // make null move
            if(depth % 2 == 0) b.makeNullMove();
            auto nullKey = b.zobristKey;
            res += generateMoves(b, depth-1);

            if(depth % 2 == 0){
                assert(b.zobristKey == nullKey);
                b.undoNullMove();
            }
            b.undoMove(moves[j]);
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

    static void runTests(){

        std::cout << "RUNNING NULL MOVES TESTS" << std::endl;
        // r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/1PN2Q1p/P1PBBPPP/R3K2R b KQkq - 0 7
        runCase("r3k3/pP3pp1/1N1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2r w Qq - 0 5", 1 );
        runCase("r3k2r/p1ppqpb1/bn2p1p1/3PN3/1p2P1n1/5Q1p/PPPBBPPP/R2NK2R w KQkq - 2 7", 1 );
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/5Q1p/PPPBBPPP/R2NK2R b KQkq - 1 6", 2 );
        runCase("r3k2r/p1ppqpb1/bn1Ppnp1/4N3/1p2P3/2N2Q2/PPPBBPpP/R3K2R w KQkq - 0 6", 1 );
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 1 );
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/1PN2Q1p/P1PBBPPP/R3K2R b KQkq - 0 7", 3);
        runCase("r3k2r/p1ppqpb1/bn1Ppnp1/4N3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 5", 2);
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 2 );
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 3 );
        runCase("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 4 );
        runCase("r3k2r/pP3pp1/1N1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R b KQkq - 0 4", 2 );
        runCase("r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4", 1);
        runCase("r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4", 2);
        runCase("r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4", 3);
        runCase("1r2k2r/1p3pp1/Pp1pr1p1/4p1P1/P3P3/3P4/2P2PP1/1R2K2R b Kk - 0 6",1);
        runCase("1r2k2r/1p3pp1/Pp1pr1p1/4p1P1/4P3/P2P4/2P2PP1/1R2K2R w Kk - 1 6",2 );
        runCase("r3k2r/1p3pp1/Pp1pr1p1/4p1P1/4P3/P2P4/2P2PP1/1R2K2R b Kkq - 0 5",3 );
        runCase("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",5);
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