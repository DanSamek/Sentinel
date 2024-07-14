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
        {100500, 200500, 300500, 400500, 500500, 600500, },
        {100400, 200400, 300400, 400400, 500400, 600400, },
        {100300, 200300, 300300, 400300, 500300, 600300, },
        {100200, 200200, 300200, 400200, 500200, 600200, },
        {100100, 200100, 300100, 400100, 500100, 600100, },
        {100000, 200000, 300000, 400000, 500000, 600000, }
    };

    // after MVV_VLA
    static inline constexpr int KILLER_MOVES_ORDER_SCORE[2] = {90000,80000};
    static inline constexpr int TT_MOVE_ORDER_SCORE = 1000000; // best move from prev iteration, lets pick it first!

public:

    /***
     * Score current moves
     *  NOW:
     *  - MVV_VLA
     *  - Promotion's
     *  - Killer moves
     *  - TT moves
     * @param moves from movegen
     * @param cnt from movegen
     * @param board current board.
     */
    static void scoreMoves(Move* moves, int cnt, Board& board, const Move killerMoves[Board::MAX_DEPTH][2], int history[64][64], const Move& hashMove, std::vector<int>& moveScores);

    /***
     * Picks a best move with a biggest score
     * @note We dont sort moves, because it can be pretty slow - we can pick first move and after that it can be pruned and no more moves will be played => waste of time
     * @param moves
     * @param cnt
     * @param startingIndex
     */
    static void pickMove(Move* moves, int cnt, int startingIndex, std::vector<int>& moveScores);
};


#endif //SENTINEL_MOVEPICK_H
