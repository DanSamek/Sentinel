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
#include "tunables.h"

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

    Board* _board;
    bool _forceStopped = false;

    Timer _timer;

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
            int delta = ASPIRATION_DELTA_START.current;
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
                if(delta >= ASPIRATION_MAX_DELTA_SIZE.current){
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
            int delta = ASPIRATION_DELTA_START.current;
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
                if(delta >= ASPIRATION_MAX_DELTA_SIZE.current){
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

        const auto isSingular = ss[ply].excludedMove != NO_MOVE;

        const TranspositionTable::Entry* entry = &TranspositionTable::NO_ENTRY;
        auto ttIndex =  TT->index(_board->zobristKey);
        auto ttEval = TranspositionTable::LOOKUP_ERROR;

        if(!isSingular){
            ttEval = TT->getEval(_board->zobristKey, ttIndex, depth, alpha, beta, ply);
            entry = TT->getEntry(ttIndex);

            if(ttEval > TranspositionTable::LOOKUP_ERROR && !isPv){
                ss.ttUsed++;
                return ttEval;
            }
        }
        // IIR
        if(ttEval == TranspositionTable::LOOKUP_ERROR && ply > 0 && depth >= IIR_DEPTH.current) depth--;

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

        int currentEval = ss[ply].score =_board->eval();

        // Improving heuristic
        const auto improving = ply >= 2 && currentEval > ss[ply - 2].score + IMPROVING_CONSTANT.current && !isCheckNMP;

        if(!isSingular){
            // Null move pruning
            // We just give enemy next move (we don't move any piece)
            // If our position is too good, even 1 additional move for opponent cant help, we return beta.
            bool someBigPiece = _board->anyBiggerPiece(); // Zugzwang prevention, in some simple endgames can NMP hurt.

            if(!isPv && depth >= NMP_DEPTH.current && doNull && !isCheckNMP && someBigPiece && ply > 0){
                _board->makeNullMove();
                ss[ply].move = NO_MOVE;

                int R = NMP_REDUCTION_CONSTANT.current + depth / NMP_REDUCTION_DIV_CONSTANT.current;
                int eval = -negamax(depth - R + 1, ply + 1, -beta, -beta + 1, false, false);
                _board->undoNullMove();

                if(eval >= beta) return eval;
                if(_forceStopped) return 0;
            }

            // Reverse futility pruning
            // If current pos - margin is too good (>= beta), we can return currentEval.
            if(!isPv && !isCheckNMP && ply > 0 && depth <= RFP_DEPTH.current && currentEval - RFP_CONSTANT.current * depth >= beta){
                return currentEval;
            }
        }

        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto [moveCount, isCheck] = Movegen(*_board, moves).generateMoves<false>();
        std::vector<int> moveScores(moveCount);

        // "move ordering"
        auto prevMove = ply == 0 ? NO_MOVE : ss[ply - 1].move;
        auto counterMove = prevMove.fromSq == -1 ? NO_MOVE : hist.counterMoves[prevMove.fromSq][prevMove.toSq];
        Movepick::scoreMoves(moves, moveCount, *_board, hist, entry->best, counterMove, moveScores);
        bool visitedAny = false;

        TranspositionTable::HashType TTType = TranspositionTable::UPPER_BOUND;
        Move bestMoveInPos;

        int movesSearched = 0; // LMR
        int quietMovesCount = 0;

        // Singular extensions -- condition
        const auto canSingular = !isSingular && ply > 0 && depth >= SI_DEPTH.current && entry->flag != TranspositionTable::UPPER_BOUND && entry->depth + SI_DEPTH_TT_ADD.current >= depth && std::abs(ttEval) < CHECKMATE_LOWER_BOUND;

        for(int j = 0; j < moveCount; j++){
            // pick a move to play (sorting moves, can be slower, thanks to alpha beta pruning).
            Movepick::pickMove(moves, moveCount, j, moveScores);

            if(moves[j] == ss[ply].excludedMove){
                continue;
            }

            bool isCapture = moves[j].isCapture();

            /* Move pruning techniques */

            if(!isCapture && ply > 0 && !isCheckNMP){

                // Late move pruning.
                auto lmpLimit = LMR_MOVE_COUNT_ADD.current + depth * depth;
                if(!isPv && quietMovesCount > lmpLimit && depth <= LMP_DEPTH.current && !moves[j].isPromotion()){
                    continue;
                }

                // Futility pruning.
                if(!isPv && !isSingular && movesSearched > 0 && depth <= FP_DEPTH.current && currentEval + FP_CONSTANT.current * depth <= alpha){
                    continue;
                }

                // SEE pruning of quiet moves.
                if(depth <= SEE_QUIET_DEPTH.current && alpha > -CHECKMATE && !_board->SEE(moves[j], -SEE_QUIET_THRESHOLD.current*depth)){
                    continue;
                }
            }

            // SEE pruning of captures.
            else if(ply > 0 && depth <= SEE_CAP_DEPTH.current && isCapture && !_board->SEE(moves[j], -SEE_CAP_THRESHOLD.current*depth*depth)){
                continue;
            }

            // Singular extensions
            // If TT move is better than other moves, extend search depth [if conditions are met] for this move.
            auto newDepth = depth;
            if(canSingular && moves[j] == entry->best){
                const auto singularBeta = ttEval - depth*SI_DEPTH_MUL.current;

                ss[ply].excludedMove = entry->best;
                const auto singularEval = negamax((depth - 1) / 2, ply, singularBeta - 1, singularBeta, true, false);
                ss[ply].excludedMove = NO_MOVE;

                if(singularEval < singularBeta){
                    newDepth++;
                    // TODO double extension.
                    // newDepth += singularEval + 25 < singularBeta && !isPv;
                }

                // TODO negative extension.

                // Multicut.
                if(singularBeta >= beta){
                    return singularBeta;
                }

            }

            if(!_board->makeMove(moves[j])) continue; // pseudolegal movegen.

            ss[ply].move = moves[j];
            TT->prefetch(TT->index(_board->zobristKey));

            // Late move reductions
            int eval;
            int R = 0;
            if(depth > LMR_DEPTH.current && ply > 0){
                R = lmrTable.data[depth][movesSearched];
                R -= isPv;
                R -= isCheck;
                R -= improving;

                R += !isCapture && entry->best.isCapture();
                R = std::clamp(R, 0, depth - 2);
            }

            if(movesSearched == 0 && isPv){
                eval = -negamax(newDepth - 1, ply + 1, -beta, -alpha, true, isPv);
            }
            else{
                // do reduced search (null window)
                eval = -negamax(newDepth - 1 - R, ply + 1, -alpha - 1, -alpha, true, false);

                // do non reduced search (null window)
                if(eval > alpha && R != 0) eval = -negamax(newDepth - 1, ply + 1, -alpha - 1, -alpha, true, false);

                // if LMR fails, do normal full search.
                if(eval > alpha && eval < beta) eval = -negamax(newDepth - 1, ply + 1, -beta, -alpha, true, isPv);
            }

            _board->undoMove();
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

                if(!isSingular) TT->store(_board->zobristKey, ttIndex,eval, depth, TranspositionTable::LOWER_BOUND, moves[j], ply);
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


        if(!isSingular) TT->store(_board->zobristKey, ttIndex, alpha, depth, TTType, bestMoveInPos, ply );
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

        auto ttIndex =  TT->index(_board->zobristKey);
        auto ttEval = TT->getEval(_board->zobristKey, ttIndex, 0, alpha, beta, ply);
        auto hashMove = ttEval == TranspositionTable::FOUND_NOT_ACCEPTED ? TT->entries[ttIndex].best : NO_MOVE;

        if(ttEval > TranspositionTable::LOOKUP_ERROR){
            ss.ttUsed++;
            return ttEval;
        }

        auto currentEval = _board->eval();
        if(currentEval >= beta) return beta;
        if(currentEval > alpha) alpha = currentEval;

        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto [moveCount, isCheck] = Movegen(*_board, moves).generateMoves<true>();
        std::vector<int> moveScores(moveCount);
        Movepick::scoreMovesQSearch(moves, moveCount, *_board, hashMove, moveScores);

        TranspositionTable::HashType TTType = TranspositionTable::UPPER_BOUND;
        Move bestMoveInPos = NO_MOVE;

        for(int j = 0; j < moveCount; j++){
            // pick the best move to play.
            Movepick::pickMove(moves, moveCount, j, moveScores);

            // SEE pruning of losing captures.
            if(!_board->SEE(moves[j], 0)) continue;

            if(!_board->makeMove(moves[j])) continue; // pseudolegal movegen.

            TT->prefetch(TT->index(_board->zobristKey));

            int eval = -qsearch(-beta, -alpha, ply + 1);
            _board->undoMove();

            if(eval >= beta){
                TT->store(_board->zobristKey, ttIndex, eval, 0, TranspositionTable::LOWER_BOUND, moves[j], ply);
                return beta;
            }
            if(eval > alpha){
                bestMoveInPos = moves[j];
                TTType = TranspositionTable::EXACT;
                alpha = eval;
            }
        }

        TT->store(_board->zobristKey, ttIndex, alpha, 0, TTType, bestMoveInPos, ply);
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



