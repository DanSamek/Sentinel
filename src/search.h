#ifndef SENTINEL_SEARCH_H
#define SENTINEL_SEARCH_H

#include <movegen.h>
#include <limits.h>
#include <movepick.h>

class Search {
    static constexpr int positiveInf = 100000000;
    static constexpr int negativeInf = -100000000;

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

            int score = -negamax(12, board, negativeInf, positiveInf);
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
    static constexpr int CHECKMATE = 1000000;
    /*
     * TODO
     * 1) qsearch (generate all captures) - DONE
     * 2) moveorder / movepick (simple (by captured piece) + promotions). DONE
     * 3) transposition table
     * ---
     * 4) iterative deepening + uci parsing with time, valid times.
     * 5) Move order - add best first move from iterative deepening.
     * 6) Iterative deepening update - if mate was found in iteration - smallest depth, we can stop.
     */

    // https://en.wikipedia.org/wiki/Negamax
    static int negamax(int depth, Board& board, int alpha, int beta){
        if(board.isDraw()) return 0;
        if(depth == 0) return qsearch(board, alpha, beta);

        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto [moveCount, isCheck] = Movegen::generateMoves(board, moves);

        // "move ordering"
        Movepick::scoreMoves(moves, moveCount, board);
        bool visitedAny = false;
        for(int j = 0; j < moveCount; j++){
            // pick a move to play.
            Movepick::pickMove(moves, moveCount, j);

            if(!board.makeMove(moves[j])) continue; // pseudolegal movegen.
            int eval = -negamax(depth - 1, board, -beta, -alpha);
            board.undoMove(moves[j]);

            visitedAny = true;

            if(eval >= beta){
                // TODO store in TT
                return beta;
            }

            if(eval > alpha){
                alpha = eval;
                // TODO TT Exact
            }
        }

        // movegen returns cnt of moves total and if king is checked.
        // if there is no move - eval == INT_MIN, we can easily return if it's draw or checkmate (for checkmate we use value < INT_MIN).
        // checkmate || draw.
        if(!visitedAny && isCheck){
            return -(CHECKMATE + depth); // checkmate.
        }
        if(!visitedAny && !isCheck) {
            return 0; // draw
        }

        // TODO store in TT.
        return alpha;
    }


    // https://www.chessprogramming.org/Quiescence_Search
    /***
     * Generates all captures possible and plays them.
     * This is used for horizon effect - i capture with a QxP -> good move, but what if another pawn in next depth will capture my Q
     *  -> qsearch.
     * @return
     */
    static int qsearch(Board& board, int alpha, int beta){
        auto currentEval = board.eval();

        if(currentEval >= beta) return beta;
        if(currentEval > alpha) alpha = currentEval;

        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto [moveCount, isCheck] = Movegen::generateMoves(board, moves, true);
        Movepick::scoreMoves(moves, moveCount, board);

        for(int j = 0; j < moveCount; j++){
            // pick a best move to play.
            Movepick::pickMove(moves, moveCount, j);

            if(!board.makeMove(moves[j])) continue; // pseudolegal movegen.
            int eval = -qsearch(board, -beta, -alpha);
            board.undoMove(moves[j]);

            if(eval >= beta) return beta;
            if(eval > alpha) alpha = eval;
        }

        return alpha;
    }
};

#endif //SENTINEL_SEARCH_H
