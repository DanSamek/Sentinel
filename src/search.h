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
            if(!board.makeMove(moves[j])) continue; // pseudolegal movegen.

            int score = -negamax(4, board);
            std::cout << score << std::endl;
            board.printBoard();

            board.undoMove(moves[j]);
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
    static int negamax(int depth, Board& board){
        if(board.isDraw()) return 0;
        if(depth == 0) return board.eval();

        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto result = Movegen::generateMoves(board, moves);

        int best = INT_MIN;
        for(int j = 0; j < result.first; j++){
            if(!board.makeMove(moves[j])) continue; // pseudolegal movegen.
            best = std::max(best, -negamax(depth - 1, board));
            board.undoMove(moves[j]);
        }

        // movegen returns cnt of moves total and if king is checked.
        // if there is no move - best == INT_MIN, we can easily return if it's draw or checkmate (for checkmate we use value < INT_MIN).
        // checkmate || draw.
        if(best == INT_MIN && !result.second) return ((board.whoPlay ? 1 : -1) * CHECKMATE) + depth; // checkmate.
        if(best == INT_MIN && result.second) return 0; // draw

        return best;
    }
};

#endif //SENTINEL_SEARCH_H
