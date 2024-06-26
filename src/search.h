#ifndef SENTINEL_SEARCH_H
#define SENTINEL_SEARCH_H

#include <movegen.h>
#include <limits.h>
#include <movepick.h>
#include <tt.h>
#include <chrono>
#include "timer.h"

class Search {
    static constexpr int positiveInf = 100000000;
    static constexpr int negativeInf = -100000000;

    static inline int bestScoreIter = INT_MIN;
    static inline Move bestMoveIter;
    static inline int MAX_DEPTH = 28;

    static inline Board* _board;
    static inline bool _forceStopped = false;

    static inline Timer _timer;


    static inline int TTUsed;
    static inline int nodesVisited;
public:
    // 8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1
    static Move search(int miliseconds, Board& board, bool exact){
        TTUsed = nodesVisited = 0;
        // get first moves, only legal.
        _board = &board;
        _forceStopped = false;
        bestScoreIter = INT_MIN;
        bestMoveIter = {};

        int msCanBeUsed = exact ? miliseconds : miliseconds / 120; // try

        _timer = Timer(msCanBeUsed);
        TT->clear(); // TODO fix!!
        Move bestMove;

        for(int j = 1; j <= MAX_DEPTH; j++){
            negamax(j, 0, negativeInf, positiveInf);
            if(_forceStopped) break;

            bestMove = bestMoveIter;
            std::cout << "depth:" << j << " score:" << bestScoreIter << " move:";
            bestMove.print();

            // thanks to iterative deepening + TT, we can stop search here - mate found.
            if(isMateScore(bestScoreIter)) break;
        }

        std::cout << "tt used:" << TTUsed << " nodesTotal:" << nodesVisited <<std::endl;
        std::cout << "tt ratio: " << (TTUsed*1.0)/nodesVisited << std::endl;
        return bestMove;
    }

    static constexpr int CHECKMATE = 1000000;
    static inline TranspositionTable* TT;
private:

    /*
     * 1) qsearch (generate all captures) - DONE
     * 2) moveorder / movepick (simple (by captured piece) + promotions). DONE
     * 3) transposition table DONE (?)
     * 4) iterative deepening + uci parsing with time, valid times. DONE
     *
     * 5) Move order - add best first move from iterative deepening.
     * 6) Iterative deepening update - if mate was found in iteration - smallest depth, we can stop. DONE
     */

    // https://en.wikipedia.org/wiki/Negamax with alpha beta + TT.
    static int negamax(int depth, int ply, int alpha, int beta){
        if(_timer.isTimeout()){
            _forceStopped = true;
            return 0;
        }

        nodesVisited++;

        if(_board->isDraw()) return 0;
        if(depth == 0) return qsearch(alpha, beta);

        // Try get eval from TT.
        int ttEval = TT->getEval(depth, alpha, beta);

        if(ttEval != TranspositionTable::LOOKUP_ERROR){
            if(ply == 0){
                bestMoveIter = TT->getMove();
                bestScoreIter = ttEval;
            }
            TTUsed++;
            return ttEval;
        }

        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto [moveCount, isCheck] = Movegen::generateMoves(*_board, moves);

        // "move ordering"
        Movepick::scoreMoves(moves, moveCount, *_board);
        bool visitedAny = false;

        TranspositionTable::HashType TTType = TranspositionTable::ALPHA;
        Move bestMoveInPos;
        for(int j = 0; j < moveCount; j++){
            // pick a move to play (sorting moves, can be slower, thanks to alpha beta pruning).
            Movepick::pickMove(moves, moveCount, j);

            if(!_board->makeMove(moves[j])) continue; // pseudolegal movegen.
            int eval = -negamax(depth - 1, ply + 1, -beta, -alpha);
            _board->undoMove(moves[j]);

            // ! after undo move !
            if(_forceStopped){
                return 0;
            }

            visitedAny = true; // for draw / checkmate checking.

            if(eval >= beta){
                TT->store(beta, depth, _board->zobristKey, TranspositionTable::BETA, moves[j]);
                return beta;
            }

            if(eval > alpha){
                alpha = eval;

                TTType = TranspositionTable::EXACT;
                bestMoveInPos = moves[j];
                // best move for current player in depth "0".
                if(ply == 0){
                    bestMoveIter = moves[j];
                    bestScoreIter = eval;
                }
            }
        }

        // movegen returns cnt of moves total and if king is checked.
        // if there is no move - eval == INT_MIN, we can easily return if it's draw or checkmate (for checkmate we use value < INT_MIN).
        // checkmate || draw.
        if(!visitedAny && isCheck) return -(CHECKMATE + depth); // checkmate.
        if(!visitedAny && !isCheck) return 0; // draw

        TT->store(alpha, depth, _board->zobristKey, TTType, bestMoveInPos);
        return alpha;
    }


    // https://www.chessprogramming.org/Quiescence_Search
    /***
     * Generates all captures possible and plays them.
     * This is used for horizon effect - i capture with a QxP -> good move, but what if another pawn in next depth will capture my Q
     *  -> qsearch.
     * @return
     */
    static int qsearch(int alpha, int beta){
        auto currentEval = _board->eval();

        if(currentEval >= beta) return beta;
        if(currentEval > alpha) alpha = currentEval;

        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto [moveCount, isCheck] = Movegen::generateMoves(*_board, moves, true);
        Movepick::scoreMoves(moves, moveCount, *_board);

        for(int j = 0; j < moveCount; j++){
            // pick a best move to play.
            Movepick::pickMove(moves, moveCount, j);

            if(!_board->makeMove(moves[j])) continue; // pseudolegal movegen.
            int eval = -qsearch(-beta, -alpha);
            _board->undoMove(moves[j]);

            if(eval >= beta) return beta;
            if(eval > alpha) alpha = eval;
        }

        return alpha;
    }

    static bool isMateScore(int score){
        return std::abs(score) >= CHECKMATE;
    }
};

#endif //SENTINEL_SEARCH_H



