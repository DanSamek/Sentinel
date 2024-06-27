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
        {20, 22, 24, 27, 30, 0},
        {19, 21, 23, 26, 29, 0},
        {18, 20, 22, 25, 28, 0},
        {17, 19, 21, 24, 27, 0},
        {16, 18, 20, 23, 26, 0},
        {15, 17, 19, 22, 25, 0},
    };
public:

    /***
     * Score current moves
     *  NOW:
     *  - MVV_VLA
     *  - Promotion
     *  TODO:
     *  - Best move from prev iteration (iterative deepening).
     *  - Killer moves
     * @param moves from movegen
     * @param cnt from movegen
     * @param board current board.
     */
    static void scoreMoves(Move* moves, int cnt, Board& board);

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
