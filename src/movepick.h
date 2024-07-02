#ifndef SENTINEL_MOVEPICK_H
#define SENTINEL_MOVEPICK_H

#include <move.h>
#include <board.h>

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
    static constexpr uint16_t MVV_VLA[6][6] ={
        {1050, 2050, 3050, 4050, 5050, 6050, },
        {1040, 2040, 3040, 4040, 5040, 6040, },
        {1030, 2030, 3030, 4030, 5030, 6030, },
        {1020, 2020, 3020, 4020, 5020, 6020, },
        {1010, 2010, 3010, 4010, 5010, 6010, },
        {1000, 2000, 3000, 4000, 5000, 6000, }
    };

    // after MVV_VLA
    static inline constexpr uint16_t KILLER_MOVES_ORDER_SCORE[2] = {900,800};
    static inline constexpr uint16_t TT_MOVE_ORDER_SCORE = 10000; // best move from prev iteration, lets pick it first!

public:

    /***
     * Score current moves
     *  NOW:
     *  - MVV_VLA
     *  - Promotion
     *  - Killer moves
     *  - TT moves
     *  TODO
     *  - PV MOVES
     * @param moves from movegen
     * @param cnt from movegen
     * @param board current board.
     */
    static void scoreMoves(Move* moves, int cnt, Board& board, const Move killerMoves[Board::MAX_DEPTH][2], const Move& hashMove);

    /***
     * Picks a best move with a biggest score
     * @note We dont sort moves, because it can be pretty slow - we can pick first move and after that it can be pruned and no more moves will be played => waste of time
     * @param moves
     * @param cnt
     * @param startingIndex
     */
    static void pickMove(Move* moves, int cnt, int startingIndex);
};


#endif //SENTINEL_MOVEPICK_H
