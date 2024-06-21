#ifndef SENTINEL_SEARCH_H
#define SENTINEL_SEARCH_H

#include <movegen.h>
#include <limits.h>

class Search {
public:
    static Move search(int timeRemaining, Board& board){
        // get first moves, only legal.
        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto result = Movegen::generateMoves(board, moves);

        int bestScore = INT_MIN;
        Move bestMove;
        for(int j = 0; j < result.first; j++){
            // play moves.
            int score = negamax(3,0,0, board);
            // its better?
            if(score > bestScore){
                bestScore = score;
                bestMove = moves[j];
            }
        }
        return bestMove;
    }

private:
    static constexpr int CHECKMATE = 100000000;
    static int negamax(int depth, int alpha, int beta, Board& board){
        if(depth == 0) return board.eval();
        int best = INT_MIN;

        // kinda sussy move gen, but it's for a performance.
        // used in perftTests.cpp ~~20-30 MNps (little bit faster +(5-10) MNps)
        // It's from movegen implementation, but used here.

        Move moves[Movegen::MAX_LEGAL_MOVES];
        // generate non legal moves, validate them here.
        auto result = Movegen::generateMoves(board, moves, false);

        for(int j = 0; j < result.first; j++){
            if(moves[j].moveType == Move::CASTLING){
                UPDATE_BOARD_STATE(board, board.whoPlay);
                int kingPos = bit_ops::bitScanForward(friendlyBits[Board::KING]);
                VALIDATE_KING_CHECKS(kingPos, board, moves, j, enemyBits);
            }

            board.makeMove(moves[j]);


            UPDATE_BOARD_STATE(board, !board.whoPlay);
            int kingPos = bit_ops::bitScanForward(friendlyBits[Board::KING]);
            bool valid = Movegen::validateKingCheck(kingPos, !board.whoPlay, enemyBits);


            if(valid) best = std::max(best, -negamax(depth - 1, alpha , beta, board));


            board.undoMove(moves[j]);
        }

        // movegen returns cnt of moves total and if king is checked.
        // if there is no move - best == INT_MIN, we can easily return if it's draw or checkmate (for checkmate we use value < INT_MIN).
        // checkmate || draw.
        if(best == INT_MIN && !result.second) return (board.whoPlay ? 1 : -1) * CHECKMATE; // checkmate.
        if(best == INT_MIN && result.second) return 0; // draw

        return best;
    }
};

#endif //SENTINEL_SEARCH_H
