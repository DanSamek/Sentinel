#include <string>
#include <vector>
#include "board.h"
#include "movegen.h"

struct SEETests{
    struct test{
        test(const std::string &fen, int from, int to, int resultValue) : FEN(fen), from(from), to(to), result(resultValue >= 0),  resultValue(resultValue) {}

        std::string FEN;
        int from;
        int to;
        bool result;
        int resultValue;
    };

    static void run(){
        // motor tests, thanks martin!
        std::vector<test> tests = {
                test("7r/5qpk/2Qp1b1p/1N1r3n/BB3p2/5p2/P1P2P2/4RK1R w - - 0 1", 60,04,-500),
                test("6k1/1pp4p/p1pb4/6q1/3P1pRr/2P4P/PP1Br1P1/5RKN w - - 0 1", 51, 37, -100),
                test("5rk1/1pp2q1p/p1pb4/8/3P1NP1/2P5/1P1BQ1P1/5RK1 b - - 0 1",19, 37, 0),
                test("1r3r1k/p4pp1/2p1p2p/qpQP3P/2P5/3R4/PP3PP1/1K1R4 b - - 0 1",24, 48, -800),
                test("1k1r3q/1ppn3p/p4b2/4p3/8/P2N2P1/1PP1R1BP/2K1Q3 w - - 0 1", 43, 28,-200),
                test("r2qk1nr/ppp1ppbp/2b3p1/2p1p3/8/2N2N2/PPPP1PPP/R1BQR1K1 w kq - 0 1", 45,28,100)
        };

        for(const auto& test : tests){
            Board b;
            b.loadFEN(test.FEN);

            Move moves[Movegen::MAX_LEGAL_MOVES];
            auto cnt = Movegen::generateMoves(b, moves);
            Move m;
            while(--cnt.first >= 0){
                if(moves[cnt.first].fromSq == test.from && moves[cnt.first].toSq == test.to){
                    m = moves[cnt.first];
                    break;
                }
            }
            auto result = b.SEE(m, 0);
            assert(result == test.result);
        }
    }
};