#ifndef SENTINEL_SEARCH_H
#define SENTINEL_SEARCH_H

#include <movegen.h>
#include <limits.h>
#include <movepick.h>
#include <tt.h>
#include <chrono>
#include <timer.h>
#include <timemanager.h>
#include <development.h>

class Search {
    static constexpr int POSITIVE_INF = 100000000;
    static constexpr int NEGATIVE_INF = -100000000;

    static inline constexpr int LMR_DEPTH = 2;

    static inline constexpr int ASPIRATION_DELTA_START = 7;
    static inline constexpr int ASPIRATION_MAX_DELTA_SIZE = 4'096;

    // !!!! Same value has to be in TT.h !!!!
    static inline constexpr int MAX_DEPTH = 128;

    Board* _board;
    bool _forceStopped = false;

    Timer _timer;

    // Debug.
    int _ttUsed;
    int _nodesVisited;

    int _bestScoreIter = INT_MIN;
    Move _bestMoveIter;

    // Killer moves, that did beta cutoffs, use them in move order.
    // Now only 2 killer moves per ply.
    Move _killerMoves[MAX_DEPTH][2];
    int _history[64][64];

    // Some moves can have natural response.
    Move _counterMoves[64][64];

    // PV
    Move _pvTable[MAX_DEPTH][MAX_DEPTH];
    int _pvLength[MAX_DEPTH];

    // !!!! Same value has to be in TT.h !!!!
    static constexpr int CHECKMATE = 1000000;
    static constexpr int CHECKMATE_LOWER_BOUND = 1000000 - 1000;


    static inline Move NO_MOVE = Move();

public:
    static inline TranspositionTable* TT;
    Move findBestMove(int timeRemaining, int increment, Board& board, bool exact, int maxDepth, bool inf){
#if DEVELOPMENT
        _ttUsed = _nodesVisited = 0;
#endif
        prepareForSearch();

        _board = &board;
        _forceStopped = false;
        _bestScoreIter = INT_MIN;
        _bestMoveIter = {};

        auto msCanBeUsed = Timemanager::getSearchTime(timeRemaining, increment, exact);

        _timer = Timer(msCanBeUsed, inf);

        int alpha = NEGATIVE_INF;
        int beta = POSITIVE_INF;
        int score;

        Move bestMove;
        Timer idTimer; // info about time.
        maxDepth = maxDepth <= 0 ? MAX_DEPTH : maxDepth + 1;

        for(int depth = 1; depth < maxDepth; depth++){
            // for smaller search do a non aspirations.
            if(depth <= 5){
                score = negamax(depth, 0, alpha, beta, true, true);
                bestMove = _bestMoveIter;
                printInfo(depth, idTimer, bestMove);
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
                    bestMove = _bestMoveIter;
                    printInfo(depth, idTimer, bestMove);
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

private:

    void prepareForSearch(){
        for (auto& killers : _killerMoves) {
            std::fill(std::begin(killers), std::end(killers), Move());
        }

        for (auto& counterMoves : _counterMoves) {
            std::fill(std::begin(counterMoves), std::end(counterMoves), Move());
        }

        for (auto& row : _history) {
            std::fill(std::begin(row), std::end(row), 0);
        }

        std::fill(std::begin(_pvLength), std::end(_pvLength), 0);
    }


    // https://en.wikipedia.org/wiki/Negamax ,PVS, alpha beta, TT, ...
    int negamax(int depth, int ply, int alpha, int beta, bool doNull, bool isPv, const Move& prevMove = NO_MOVE){
        if(_timer.isTimeout()){
            _forceStopped = true;
            return 0;
        }
        assert(isPv || alpha + 1 == beta);
        _nodesVisited++;

        // Pv.
        _pvLength[ply] = ply;


        // Check extension.
        if(ply > MAX_DEPTH - 1) return _board->eval();

        if(_board->isDraw()) return 0;

        // Try get eval from TT.
        auto ttIndex =  TT->index(_board->zobristKey);
        int ttEval = TT->getEval(_board->zobristKey, ttIndex, depth, alpha, beta, ply);
        auto hashMove = ttEval == TranspositionTable::FOUND_NOT_ACCEPTED ? TT->entries[ttIndex].best : Move();

        if(ttEval > TranspositionTable::LOOKUP_ERROR && !isPv){
            _ttUsed++;
            return ttEval;
        }
        // IIR
        if(ttEval == TranspositionTable::LOOKUP_ERROR && ply > 0 && depth >= 5) depth--;

        if(depth <= 0)
        {
            _nodesVisited--;
            return qsearch(alpha, beta, ply);
        }
        bool isCheckNMP = _board->inCheck(); // If current king is checked, logically we can't do NMP (enemy will capture our king).

        // Check extension.
        if(isCheckNMP) depth++;

        int currentEval = _board->eval();

        // Null move pruning
        // We just give enemy next move (we dont move any piece)
        // If our position is too good, even 1 additional move for opponent cant help, we return beta.
        bool someBigPiece = _board->anyBiggerPiece(); // Zugzwang prevention, in some simple endgames can NMP hurt.

        if(!isPv && depth >= 3 && doNull && !isCheckNMP && someBigPiece && ply > 0){
            _board->makeNullMove();
            int eval = -negamax(depth - 3, ply + 1, -beta, -beta + 1, false, false);
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
        auto counterMove = prevMove.fromSq == -1 ? NO_MOVE : _counterMoves[prevMove.fromSq][prevMove.toSq];
        Movepick::scoreMoves(moves, moveCount, *_board, _killerMoves, _history ,hashMove, counterMove,moveScores);
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

                // SEE pruning of quiet moves.
                if(depth <= 7 && alpha > -CHECKMATE && !_board->SEE(moves[j], -80*depth)){
                    continue;
                }

            }
            // SEE pruning of captures.
            // Dont prune so much captures, we can still be in good position even if we lose material in SEE (sacrifice for example).
            else if(ply > 0 && depth <= 7 && isCapture && !_board->SEE(moves[j], -40*depth*depth)){
                continue;
            }

            if(!_board->makeMove(moves[j])) continue; // pseudolegal movegen.

            // late move reduction.
            int eval = lmr(depth, ply, alpha, beta, movesSearched, isPv, moveScores[j], moves[j]);

            _board->undoMove(moves[j]);
            quietMovesCount += !isCapture;

            // ! after undo move !
            if(_forceStopped){
                return 0;
            }

            // update PV
            if(eval > alpha){
                _pvTable[ply][ply] = moves[j];
                for (int index = ply + 1; index < _pvLength[ply + 1]; index++) {
                    _pvTable[ply][index] = _pvTable[ply + 1][index];
                }
                _pvLength[ply] = _pvLength[ply + 1];
            }

            if(eval >= beta){
                // If move, that wasnt capture causes a beta cuttoff, we call it killer move, remember this move for move ordering.
                if(!isCapture){
                    storeKillerMove(ply, moves[j]);
                    _history[moves[j].fromSq][moves[j].toSq] += depth * depth;

                    if(ply > 0 && prevMove != NO_MOVE){
                        _counterMoves[prevMove.fromSq][prevMove.toSq] = moves[j];
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
                    _bestMoveIter = moves[j];
                    _bestScoreIter = eval;
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


    inline int lmr(int depth, int ply, int alpha, int beta, int movesSearched, bool isPv, int moveScore, const Move& move) {
        int eval;

        int R = 0;
        if(depth > LMR_DEPTH && ply > 0){
            R += !isPv;
            R += moveScore == 0;
            R += movesSearched >= 4;

            R = std::clamp(R, 0, depth - 2);
        }
        if(movesSearched == 1 && isPv){
            eval = -negamax(depth - 1, ply + 1, -beta, -alpha, true, isPv, move);
        }
        else{
            // do reduced search (null window)
            eval = -negamax(depth - 1 - R, ply + 1, -alpha - 1, -alpha, true, false, move);

            // do non reduced search (null window)
            if(eval > alpha && R != 0) eval = -negamax(depth - 1, ply + 1, -alpha - 1, -alpha, true, false, move);

            // if LMR fails, do normal full search.
            if(eval > alpha && eval < beta) eval = -negamax(depth - 1, ply + 1, -beta, -alpha, true, isPv, move);
        }
        return eval;
    }

    /***
     * https://www.chessprogramming.org/Quiescence_Search
     * Generates all captures possible and plays them.
     * This is used for horizon effect - If i capture with a QxP -> good move, but what if another pawn in next depth will capture my Q?!
     *  -> qsearch.
     * @return eval of the position without captures.
     */
    int qsearch(int alpha, int beta, int ply){
        _nodesVisited++;

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
            // pick a best move to play.
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

    inline void storeKillerMove(int ply, const Move& move){
        _killerMoves[ply][1] = _killerMoves[ply][0];
        _killerMoves[ply][0] = move;
    }


    void printInfo(int depth, Timer idTimer, Move& bestMove){
        auto ms = idTimer.getMs() / 1000.0;
        auto nps = _nodesVisited / (ms == 0 ? 1 : ms);

        int movesToCheckmate = 0;
        if (_bestScoreIter <= -(CHECKMATE - MAX_DEPTH)) {
            int plyToCheckmate = CHECKMATE + _bestScoreIter;
            movesToCheckmate = -((plyToCheckmate + 1) / 2);
        } else if (_bestScoreIter >= (CHECKMATE - MAX_DEPTH)) {
            int plyToCheckmate = CHECKMATE - _bestScoreIter;
            movesToCheckmate = (plyToCheckmate + 1) / 2;
        }
        if(movesToCheckmate != 0 && _bestScoreIter != INT_MIN) std::cout << "info score mate " << movesToCheckmate;
        else std::cout << "info score cp " << _bestScoreIter;

        std::cout << " depth " << depth << " nodes " << _nodesVisited << " time " << idTimer.getMs() << " nps " << (int)nps << " move ";
        bestMove.print();

        // PV
        std::cout << " pv ";
        for(int j = 0; j < _pvLength[0]; j++){
            _pvTable[0][j].print();
            std::cout << " ";
        }
        std::cout << std::endl;
    }
};

#endif //SENTINEL_SEARCH_H



