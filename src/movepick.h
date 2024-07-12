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
        {10500, 20500, 30500, 40500, 50500, 60500, },
        {10400, 20400, 30400, 40400, 50400, 60400, },
        {10300, 20300, 30300, 40300, 50300, 60300, },
        {10200, 20200, 30200, 40200, 50200, 60200, },
        {10100, 20100, 30100, 40100, 50100, 60100, },
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
