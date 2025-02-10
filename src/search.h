#ifndef SENTINEL_SEARCH_H
#define SENTINEL_SEARCH_H

#include <movegen.h>
#include <climits>
#include <movepick.h>
#include <tt.h>
#include <chrono>
#include <timer.h>
#include <timemanager.h>
#include <development.h>
#include "consts.h"
#include "history.h"
#include "searchstack.h"

class Search {
    struct LMRTable{
        int data[MAX_DEPTH][Movegen::MAX_LEGAL_MOVES];
        bool loaded;

        LMRTable(bool loaded) : loaded(loaded){}

        void init(){
            if(loaded) return;
            for(int depth = 1; depth <= MAX_DEPTH; depth++){
                for(int moveCnt = 1; moveCnt <= Movegen::MAX_LEGAL_MOVES; moveCnt++){
                    data[depth-1][moveCnt-1] = round(0.65 + log(depth) * log(moveCnt) * 0.3);
                }
            }
            loaded = true;
        }
    };

    static inline LMRTable lmrTable = LMRTable(false);

    static constexpr int POSITIVE_INF = 100000000;
    static constexpr int NEGATIVE_INF = -100000000;

    static inline constexpr int LMR_DEPTH = 2;

    static inline constexpr int ASPIRATION_DELTA_START = 15;
    static inline constexpr int ASPIRATION_MAX_DELTA_SIZE = 481;

    Board* _board;
    bool _forceStopped = false;

    Timer _timer;

    static inline auto NO_MOVE = Move();

    History hist = History();
    SearchStack ss;



public:
    TranspositionTable* TT;
    Move findBestMove(int timeRemaining, int increment, Board& board, bool exact, int maxDepth, bool inf){
#if DEVELOPMENT
        _ttUsed = _nodesVisited = 0;
#endif
        prepareForSearch();

        _board = &board;
        _forceStopped = false;

        auto msCanBeUsed = Timemanager::getSearchTime(timeRemaining, increment, exact);

        _timer = Timer(msCanBeUsed, inf);

        int alpha = NEGATIVE_INF;
        int beta = POSITIVE_INF;
        int score;

        Timer idTimer; // info about time.
        maxDepth = maxDepth <= 0 ? MAX_DEPTH : maxDepth + 1;
        Move bestMove;

        for(int depth = 1; depth < maxDepth; depth++){
            // for smaller search do a non aspirations.
            if(depth <= 5){
                score = negamax(depth, 0, alpha, beta, true, true);
                bestMove = ss.bestMove;
                printInfo(depth, idTimer);
                if(_forceStopped) break;

                continue;
            }

            // Aspiration windows
            int delta = ASPIRATION_DELTA_START;
            alpha = std::max(NEGATIVE_INF, score - delta);
            beta = std::min(POSITIVE_INF, score + delta);

            int reduction = 0;
            while(!_forceStopped){
                score = negamax(depth - reduction, 0, alpha, beta, true, true);
                if(score <= alpha && score > -CHECKMATE_LOWER_BOUND){
                    alpha -= delta;
                    beta = (alpha + beta) / 2;
                }
                else if(score >= beta && score < CHECKMATE_LOWER_BOUND){
                    beta += delta;
                }
                else{
                    if(!_forceStopped){
                        bestMove = ss.bestMove;
                        printInfo(depth, idTimer);
                    }
                    break;
                }
                delta *= 2;
                if(delta >= ASPIRATION_MAX_DELTA_SIZE){
                    alpha = NEGATIVE_INF;
                    beta = POSITIVE_INF;
                }
            }
        }

#if DEVELOPMENT
        std::cout << "TT used:" << _ttUsed << " _nodesVisited:" << _nodesVisited << std::endl;
#endif
        return bestMove;
    }

    std::pair<Move, int> datagen(Board& board, int softNodeLimit){
        prepareForSearch();

        _board = &board;
        _timer = Timer(0, true);

        int alpha = NEGATIVE_INF;
        int beta = POSITIVE_INF;
        int score;

        for(int depth = 1; depth < MAX_DEPTH; depth++){
            if(ss.nodesVisited > softNodeLimit) {
                break;
            }
            // for smaller search do a non aspirations.
            if(depth <= 5){
                score = negamax(depth, 0, alpha, beta, true, true);
                continue;
            }

            // Aspiration windows
            int delta = ASPIRATION_DELTA_START;
            alpha = std::max(NEGATIVE_INF, score - delta);
            beta = std::min(POSITIVE_INF, score + delta);

            int reduction = 0;
            while(!_forceStopped){
                score = negamax(depth - reduction, 0, alpha, beta, true, true);
                if(score <= alpha && score > -CHECKMATE_LOWER_BOUND){
                    alpha -= delta;
                    beta = (alpha + beta) / 2;
                }
                else if(score >= beta && score < CHECKMATE_LOWER_BOUND){
                    beta += delta;
                }
                else{
                    break;
                }
                delta *= 2;
                if(delta >= ASPIRATION_MAX_DELTA_SIZE){
                    alpha = NEGATIVE_INF;
                    beta = POSITIVE_INF;
                }
            }
        }

        return {ss.bestMove, ss.bestScore};
    }

private:

    void prepareForSearch(){
        ss = SearchStack();
        hist.init();
        lmrTable.init();

#if !RUN_DATAGEN
        std::fill(std::begin(ss.pvLength), std::end(ss.pvLength), 0);
#endif
    }

    // https://en.wikipedia.org/wiki/Negamax ,PVS, alpha beta, TT, ...
    int negamax(int depth, int ply, int alpha, int beta, bool doNull, bool isPv){
#if !RUN_DATAGEN
        if(ss.nodesVisited & 2048 && _timer.isTimeout()){
            _forceStopped = true;
            return 0;
        }

        // Pv.
        ss.pvLength[ply] = ply;
#endif
        assert(isPv || alpha + 1 == beta);
        ss.nodesVisited++;


        // Check extension.
        if(ply > MAX_DEPTH - 1) return _board->eval();

        if(_board->isDraw()) return 0;

        // Try get eval from TT.
        auto ttIndex =  TT->index(_board->zobristKey);
        int ttEval = TT->getEval(_board->zobristKey, ttIndex, depth, alpha, beta, ply);
        auto hashMove = ttEval == TranspositionTable::FOUND_NOT_ACCEPTED ? TT->entries[ttIndex].best : Move();

        if(ttEval > TranspositionTable::LOOKUP_ERROR && !isPv){
            ss.ttUsed++;
            return ttEval;
        }
        // IIR
        if(ttEval == TranspositionTable::LOOKUP_ERROR && ply > 0 && depth >= 5) depth--;

        if(depth <= 0)
        {
            ss.nodesVisited--;
            return qsearch(alpha, beta, ply);
        }

        // Mate distance pruning
        auto alphaMd = std::max(alpha, -CHECKMATE + ply);
        auto betaMd = std::min(beta, CHECKMATE - ply - 1);
        if(ply > 0 && alphaMd >= betaMd){
            return alphaMd;
        }

        bool isCheckNMP = _board->inCheck(); // If current king is checked, logically we can't do NMP (enemy will capture our king).

        // Check extension.
        if(isCheckNMP) depth++;

        int currentEval = ss.data[ply].score =_board->eval();

        // Improving heuristic
        const auto improving = ply >= 2 && currentEval > ss.data[ply - 2].score + 41 && !isCheckNMP;

        // Null move pruning
        // We just give enemy next move (we don't move any piece)
        // If our position is too good, even 1 additional move for opponent cant help, we return beta.
        bool someBigPiece = _board->anyBiggerPiece(); // Zugzwang prevention, in some simple endgames can NMP hurt.

        if(!isPv && depth >= 3 && doNull && !isCheckNMP && someBigPiece && ply > 0){
            _board->makeNullMove();
            ss.data[ply].move = NO_MOVE;

            int R = 3 + depth / 3;
            int eval = -negamax(depth - R + 1, ply + 1, -beta, -beta + 1, false, false);
            _board->undoNullMove();

            if(eval >= beta) return eval;
            if(_forceStopped) return 0;
        }

        // Reverse futility pruning
        // If current pos - margin is too good (>= beta), we can return currentEval.
        if(!isPv && !isCheckNMP && ply > 0 && depth <= 8 && currentEval - 92 * depth >= beta){
            return currentEval;
        }

        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto [moveCount, isCheck] = Movegen(*_board, moves).generateMoves<false>();
        std::vector<int> moveScores(moveCount);

        // "move ordering"
        auto prevMove = ply == 0 ? NO_MOVE : ss.data[ply - 1].move;
        auto counterMove = prevMove.fromSq == -1 ? NO_MOVE : hist.counterMoves[prevMove.fromSq][prevMove.toSq];
        Movepick::scoreMoves(moves, moveCount, *_board, hist,hashMove, counterMove, moveScores);
        bool visitedAny = false;

        TranspositionTable::HashType TTType = TranspositionTable::UPPER_BOUND;
        Move bestMoveInPos;

        int movesSearched = 0; // LMR
        int quietMovesCount = 0;
        for(int j = 0; j < moveCount; j++){
            // pick a move to play (sorting moves, can be slower, thanks to alpha beta pruning).
            Movepick::pickMove(moves, moveCount, j, moveScores);

            bool isCapture = moves[j].isCapture();

            /* Move pruning techniques */

            if(!isCapture && ply > 0 && !isCheckNMP){

                // Late move pruning.
                auto lmpLimit = 3 + depth * depth;
                if(!isPv && quietMovesCount > lmpLimit && depth <= 5 && !moves[j].isPromotion()){
                    continue;
                }

                // Futility pruning
                if(!isPv && movesSearched > 0 && depth <= 7 && currentEval + 140 * depth <= alpha){
                    continue;
                }

                // SEE pruning of quiet moves.
                if(depth <= 7 && alpha > -CHECKMATE && !_board->SEE(moves[j], -80*depth)){
                    continue;
                }
            }
            // SEE pruning of captures.
            // Don't prune so much captures, we can still be in good position even if we lose material in SEE (sacrifice for example).
            else if(ply > 0 && depth <= 7 && isCapture && !_board->SEE(moves[j], -40*depth*depth)){
                continue;
            }

            if(!_board->makeMove(moves[j])) continue; // pseudolegal movegen.

            ss.data[ply].move = moves[j];

            // Late move reductions
            int eval;
            int R = 0;
            if(depth > LMR_DEPTH && ply > 0){
                R = lmrTable.data[depth][movesSearched];
                R -= isPv;
                R -= isCheck;
                R -= improving;

                R += !isCapture && hashMove.isCapture();
                R = std::clamp(R, 0, depth - 2);
            }

            if(movesSearched == 0 && isPv){
                eval = -negamax(depth - 1, ply + 1, -beta, -alpha, true, isPv);
            }
            else{
                // do reduced search (null window)
                eval = -negamax(depth - 1 - R, ply + 1, -alpha - 1, -alpha, true, false);

                // do non reduced search (null window)
                if(eval > alpha && R != 0) eval = -negamax(depth - 1, ply + 1, -alpha - 1, -alpha, true, false);

                // if LMR fails, do normal full search.
                if(eval > alpha && eval < beta) eval = -negamax(depth - 1, ply + 1, -beta, -alpha, true, isPv);
            }

            _board->undoMove(moves[j]);
            quietMovesCount += !isCapture;

#if! RUN_DATAGEN
            // ! after undo move !
            if(_forceStopped){
                return 0;
            }

            // update PV
            if(eval > alpha){
                ss.pvTable[ply][ply] = moves[j];
                for (int index = ply + 1; index < ss.pvLength[ply + 1]; index++) {
                    ss.pvTable[ply][index] = ss.pvTable[ply + 1][index];
                }
                ss.pvLength[ply] = ss.pvLength[ply + 1];
            }
#endif
            if(eval >= beta){
                // If move that wasn't a capture causes a beta cutoff, we call it killer move, remember this move for move ordering.
                if(!isCapture){
                    hist.storeKillerMove(ply, moves[j]);
                    hist.updateHistory(moves[j], depth * depth);

                    if(ply > 0 && prevMove != NO_MOVE){
                        hist.storeCounterMove(prevMove, moves[j]);
                    }
                }

                TT->store(_board->zobristKey, ttIndex,eval, depth, TranspositionTable::LOWER_BOUND, moves[j], ply);
                return eval;
            }

            if(eval > alpha){
                alpha = eval;

                TTType = TranspositionTable::EXACT;
                bestMoveInPos = moves[j];
                // best move for current player in depth "0".
                if(ply == 0){
                    ss.bestMove = moves[j];
                    ss.bestScore = eval;
                }
            }

            visitedAny = true; // for draw / checkmate checking.
            movesSearched++; // for LMR
        }

        if(!visitedAny && isCheck) return -CHECKMATE + ply; // checkmate.
        if(!visitedAny && !isCheck) return 0; // draw


        TT->store(_board->zobristKey, ttIndex, alpha, depth, TTType, bestMoveInPos, ply );
        return alpha;
    }

    /***
     * https://www.chessprogramming.org/Quiescence_Search
     * Generates all captures possible and plays them.
     * This is used for horizon effect - If i capture with a QxP -> good move, but what if another pawn in next depth will capture my Q?!
     *  -> qsearch.
     * @return eval of the position without captures.
     */
    int qsearch(int alpha, int beta, int ply){
        ss.nodesVisited++;

        if(_board->isDraw()) return 0;
        if(ply >= MAX_DEPTH) return _board->eval();

        auto currentEval = _board->eval();
        if(currentEval >= beta) return beta;
        if(currentEval > alpha) alpha = currentEval;

        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto [moveCount, isCheck] = Movegen(*_board, moves).generateMoves<true>();
        std::vector<int> moveScores(moveCount);
        Movepick::scoreMovesQSearch(moves, moveCount, *_board, Move(), moveScores);

        for(int j = 0; j < moveCount; j++){
            // pick the best move to play.
            Movepick::pickMove(moves, moveCount, j, moveScores);

            // SEE pruning of losing captures.
            if(!_board->SEE(moves[j], 0)) continue;

            if(!_board->makeMove(moves[j])) continue; // pseudolegal movegen.
            int eval = -qsearch(-beta, -alpha, ply + 1);
            _board->undoMove(moves[j]);

            if(eval >= beta) return beta;
            if(eval > alpha) alpha = eval;
        }

        return alpha;
    }


    void printInfo(int depth, Timer idTimer){
        auto ms = idTimer.getMs() / 1000.0;
        auto nps = ss.nodesVisited / (ms == 0 ? 1 : ms);

        int movesToCheckmate = 0;
        if (ss.bestScore <= -(CHECKMATE - MAX_DEPTH)) {
            int plyToCheckmate = CHECKMATE + ss.bestScore;
            movesToCheckmate = -((plyToCheckmate + 1) / 2);
        } else if (ss.bestScore >= (CHECKMATE - MAX_DEPTH)) {
            int plyToCheckmate = CHECKMATE - ss.bestScore;
            movesToCheckmate = (plyToCheckmate + 1) / 2;
        }
        if(movesToCheckmate != 0 && ss.bestScore != INT_MIN) std::cout << "info score mate " << movesToCheckmate;
        else std::cout << "info score cp " << ss.bestScore;

        std::cout << " depth " << depth << " nodes " << ss.nodesVisited << " time " << idTimer.getMs() << " nps " << (int)nps << " move ";
        ss.bestMove.print();

        // PV
        std::cout << " pv ";
        for(int j = 0; j < ss.pvLength[0]; j++){
            ss.pvTable[0][j].print();
            std::cout << " ";
        }
        std::cout << std::endl;
    }
};

#endif //SENTINEL_SEARCH_H



