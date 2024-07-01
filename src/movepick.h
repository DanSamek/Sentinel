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
    static constexpr int MVV_VLA[6][6] ={
        {10050, 20050, 30050, 40050, 50050, 60050, },
        {10040, 20040, 30040, 40040, 50040, 60040, },
        {10030, 20030, 30030, 40030, 50030, 60030, },
        {10020, 20020, 30020, 40020, 50020, 60020, },
        {10010, 20010, 30010, 40010, 50010, 60010, },
        {10000, 20000, 30000, 40000, 50000, 60000, }
    };

    // after MVV_VLA
    static inline constexpr int KILLER_MOVES_ORDER_SCORE[2] = {9000,8000};
    static inline constexpr int TT_MOVE_ORDER_SCORE = 100000; // best move from prev iteration, lets pick it first!

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
