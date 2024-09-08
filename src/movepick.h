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
        {10050000, 20050000, 30050000, 40050000, 50050000, 60050000, },
        {10040000, 20040000, 30040000, 40040000, 50040000, 60040000, },
        {10030000, 20030000, 30030000, 40030000, 50030000, 60030000, },
        {10020000, 20020000, 30020000, 40020000, 50020000, 60020000, },
        {10010000, 20010000, 30010000, 40010000, 50010000, 60010000, },
        {10000000, 20000000, 30000000, 40000000, 50000000, 60000000, }
    };

    // after MVV_VLA
    static inline constexpr int KILLER_MOVES_ORDER_SCORE[2] = {9'000'000,8'000'000};
    static inline constexpr int TT_MOVE_ORDER_SCORE = 1'000'000'000; // best move from prev iteration, lets pick it first!

public:

    /***
     * Move scoring for moves in main search.
     */
    static void scoreMoves(Move* moves, int cnt, Board& board, const Move killerMoves[Board::MAX_DEPTH][2], int history[64][64], const Move& hashMove, std::vector<int>& moveScores);

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


#endif //SENTINEL_MOVEPICK_H
