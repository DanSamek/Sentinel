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
        {100500000, 200500000, 300500000, 400500000, 500500000, 600500000, },
        {100400000, 200400000, 300400000, 400400000, 500400000, 600400000, },
        {100300000, 200300000, 300300000, 400300000, 500300000, 600300000, },
        {100200000, 200200000, 300200000, 400200000, 500200000, 600200000, },
        {100100000, 200100000, 300100000, 400100000, 500100000, 600100000, },
        {100000000, 200000000, 300000000, 400000000, 500000000, 600000000, }
    };

    // after MVV_VLA
    static inline constexpr int COUNTER_MOVE_SCORE = 70'000'000;
    static inline constexpr int KILLER_MOVES_ORDER_SCORE[2] = {90'000'000,80'000'000};
    static inline constexpr int TT_MOVE_ORDER_SCORE = 1'000'000'000; // best move from prev iteration, lets pick it first!

public:

    /***
     * Move scoring for moves in main search.
     */
    static void scoreMoves(Move *moves, int cnt, Board &board,
                           const Move killerMoves[Board::MAX_DEPTH][2], int history[64][64],
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


#endif //SENTINEL_MOVEPICK_H
