#include "board.h"
#include "nnue/neuralnetwork.h"
#include "movegen.h"

struct NNUETests{
    static void diff(std::string FEN){
        Board b;
        b.loadFEN(FEN);
        auto eval = nn.eval(b);
        auto nnueEval = b.eval();
        std::cout << "NN X NNUE: " << eval << " x " << nnueEval << std::endl;
    }


    static void perft(std::string position, int depth){
        Board b;
        std::cout << position << std::endl;
        b.loadFEN(position);
        generateMoves(b, depth);
    }

    static constexpr int ALLOWED_DIFF = 20;

    static void generateMoves(Board& b, int depth){
        if(depth == 0) return;

        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto result = Movegen(b, moves).generateMoves<false>();

        int total = 0;
        for(int j = 0; j < result.first; j++){
            if(!b.makeMove(moves[j])) continue;

            auto NNEval = nn.eval(b);
            auto NNUEEval = b.eval();
            auto diff = std::abs(std::abs(NNEval) - std::abs(NNUEEval));

            if(diff > ALLOWED_DIFF){
                std::cout << "NN X NNUE: " << NNEval << " x " << NNUEEval << std::endl;
                std::cout << b.FEN() << std::endl;
                assert(false);
            }
            total ++;
            generateMoves(b, depth-1);
            b.undoMove(moves[j]);
        }
        if(total % 100 == 0 && total != 0){
            std::cout << total;
        }
    }

    static inline NeuralNetwork nn;

    static void testNNUE() {
        nn.load();

        diff("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        diff("r1bqkbnr/ppp1pppp/2n5/3p4/3P4/1BN2N2/PPP1PPPP/R1BQ1RK1 w kq - 0 1");
        diff("r2qkb1r/ppp1pppp/2n2n2/3p1b2/3P4/1BN2N2/PPP1PPPP/R1BQ1RK1 w kq - 0 1");
        diff("rnbqkbnr/pppppppp/8/3P4/4P3/1BN2N2/PPP1QPPP/R1B2RK1 w kq - 0 1");
        diff("r1bqkbnr/pppp1ppp/2n5/4p3/4P3/3P4/PPP2PPP/RNBQKBNR w KQkq - 0 1");
        diff("1kr2b1r/ppp2ppp/2np1n2/4pb2/8/8/PPPPPPPP/RNB1KBNR b KQk - 1 1");


        perft("r3k3/pP3pp1/1N1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2r w Qq - 0 5", 1);
        perft("r3k2r/p1ppqpb1/bn2p1p1/3PN3/1p2P1n1/5Q1p/PPPBBPPP/R2NK2R w KQkq - 2 7", 1);
        perft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/5Q1p/PPPBBPPP/R2NK2R b KQkq - 1 6", 2 );
        perft("r3k2r/p1ppqpb1/bn1Ppnp1/4N3/1p2P3/2N2Q2/PPPBBPpP/R3K2R w KQkq - 0 6", 1 );
        perft("8/8/8/1PpK4/5p2/4k3/8/8 b - - 0 24", 1);
        perft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 1);

        perft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/1PN2Q1p/P1PBBPPP/R3K2R b KQkq - 0 7", 3 );
        perft("r3k2r/p1ppqpb1/bn1Ppnp1/4N3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 5", 2);
        perft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 2 );
        perft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", 3 );
        perft("r3k2r/pP3pp1/1N1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R b KQkq - 0 4", 2);
        perft("r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4", 1 );
        perft("r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4", 2 );
        perft("r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4", 3 );

        perft("1r2k2r/1p3pp1/Pp1pr1p1/4p1P1/P3P3/3P4/2P2PP1/1R2K2R b Kk - 0 6",1 );
        perft("1r2k2r/1p3pp1/Pp1pr1p1/4p1P1/4P3/P2P4/2P2PP1/1R2K2R w Kk - 1 6",2 );
        perft("r3k2r/1p3pp1/Pp1pr1p1/4p1P1/4P3/P2P4/2P2PP1/1R2K2R b Kkq - 0 5",3 );
        perft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 4 );
        perft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 1);
        perft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 2 );
        perft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 3);
        perft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 5 );
        perft("rnQ1qk1r/pp3ppp/2p5/8/1bB5/8/PPP1NKPP/RNBQ3R w - - 1 10", 1 );
        perft("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NKPP/RNBQ3R b - - 0 9", 2 );
        perft("rnQq1k1r/pp3ppp/2p5/8/1bB5/2P5/PP2NnPP/RNBQK2R b KQ - 0 9",2 );
        perft("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NnPP/RNBQK2R w KQ - 1 9",1 );
        perft("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NnPP/RNBQK2R w KQ - 1 9",2 );
        perft("rnQq1k1r/pp3ppp/2p5/8/1bB5/8/PPP1NnPP/RNBQK2R w KQ - 1 9",3 );
        perft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",2 );
        perft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",3);


        perft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",2 );
        perft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",3 );
        perft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",4 );

        perft("5k2/8/8/8/8/8/8/4K2R w K - 0 1", 6);

        perft("8/P1k5/K7/8/8/8/8/8 w - - 0 1", 6 );
        perft("8/k1P5/8/1K6/8/8/8/8 w - - 0 1", 7 );

        perft("3r4/2p1p3/8/1P1P1P2/3K4/5k2/8/8 b - - 0 1", 4);

        perft("8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1", 6);
        perft("8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1", 8);


        //perft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", 6 );
        //perft("r1bq2r1/1pppkppp/1b3n2/pP1PP3/2n5/2P5/P3QPPP/RNB1K2R w KQ a6 0 12", 5);
    }

};