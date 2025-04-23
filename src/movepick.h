#ifndef SENTINEL_MOVEPICK_H
#define SENTINEL_MOVEPICK_H

#include <move.h>
#include <board.h>
#include "history.h"

namespace Sentinel{
    class Movepick {

        // attacker -> victim
        /*
          PAWN,
          KNIGHT,
          BISHOP,
          ROOK,
          QUEEN,
          KING
         */
        // MVV-LVA (most valuable victim, least valuable attacker).
        static constexpr int MVV_VLA[6][6] ={
            {100500, 200500, 300500, 400500, 500500, 600500, },
            {100400, 200400, 300400, 400400, 500400, 600400, },
            {100300, 200300, 300300, 400300, 500300, 600300, },
            {100200, 200200, 300200, 400200, 500200, 600200, },
            {100100, 200100, 300100, 400100, 500100, 600100, },
            {100000, 200000, 300000, 400000, 500000, 600000, }
        };

        // after MVV_VLA
        static inline constexpr int COUNTER_MOVE_SCORE = 70'000;
        static inline constexpr int KILLER_MOVES_ORDER_SCORE[2] = {90'000,80'000};
        static inline constexpr int TT_MOVE_ORDER_SCORE = 1'000'000; // best move from prev iteration, lets pick it first!

    public:

        /***
         * Move scoring for moves in main search.
         */
        static void scoreMoves(Move *moves, int cnt, Board &board, const History& moveHistory,
                               const Move& hashMove, const Move& counterMove, std::vector<int>& moveScores);

        /***
         * Move scoring for moves in qsearch (simplier)
         */
        static void scoreMovesQSearch(Move* moves, int cnt, Board& board, const Move& hashMove, std::vector<int>& moveScores);
        /***
         * Picks a best move with a biggest score
         * @note We dont sort moves, because it can be pretty slow - we can pick first move and after that it can be pruned and no more moves will be played => waste of time
         * @param moves
         * @param cnt
         * @param startingIndex
         */
        static void pickMove(Move* moves, int cnt, int startingIndex, std::vector<int>& moveScores);
    };

}

#endif //SENTINEL_MOVEPICK_H
