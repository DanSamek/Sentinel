#ifndef SENTINEL_SEARCH_H
#define SENTINEL_SEARCH_H

#include <movegen.h>
#include <limits.h>
#include <movepick.h>
#include <tt.h>
#include <chrono>
#include <timer.h>

class Search {
    static constexpr int POSITIVE_INF = 100000000;
    static constexpr int NEGATIVE_INF = -100000000;

    static inline int _bestScoreIter = INT_MIN;
    static inline Move _bestMoveIter;
    static inline constexpr int LMR_DEPTH = 2;

    static inline constexpr int RAZOR_MARGIN = 558;
    static inline constexpr int RAZOR_MIN_DEPTH = 4;


    static inline Board* _board;
    static inline bool _forceStopped = false;

    static inline Timer _timer;

    // Debug.
    static inline int TTUsed;
    static inline int nodesVisited;
    static inline int pvCounter;

    // Killer moves, that did beta cutoffs, use them in move order.
    // Now only 2 killer moves per ply.
    static inline Move _killerMoves[Board::MAX_DEPTH][2];
    static inline int _history[64][64];


    // static inline int EFP[1] = {125};

public:
    static inline constexpr int MAX_DEPTH = 128;
    // 8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1
    static Move search(int miliseconds, Board& board, bool exact){
        TTUsed = nodesVisited = 0;

        prepareForSearch();

        std::cout << "b hash "<< board.zobristKey << std::endl;
        // get first moves, only legal.
        _board = &board;
        _forceStopped = false;
        _bestScoreIter = INT_MIN;
        _bestMoveIter = {};

        int msCanBeUsed = exact ? miliseconds : miliseconds / 80; // try ?? 69

        _timer = Timer(msCanBeUsed);
        //TT->clear();
        Move bestMove;
        for(int depth = 1; depth < MAX_DEPTH; depth++){
            Timer idTimer;
            negamax(depth, 0, NEGATIVE_INF, POSITIVE_INF, true, true);
            if(_forceStopped){
                break;
            }
            bestMove = _bestMoveIter;

            // TODO CLEAR
            int movesToCheckmate = 0;
            if (_bestScoreIter <= -(CHECKMATE - MAX_DEPTH)) {
                int plyToCheckmate = CHECKMATE + _bestScoreIter;
                movesToCheckmate = -((plyToCheckmate + 1) / 2);
            } else if (_bestScoreIter >= (CHECKMATE - MAX_DEPTH)) {
                int plyToCheckmate = CHECKMATE - _bestScoreIter;
                movesToCheckmate = (plyToCheckmate + 1) / 2;
            }
            if(movesToCheckmate != 0 && _bestScoreIter != INT_MIN) std::cout << "info score mate " << movesToCheckmate << " depth " << depth << " time " << idTimer.getMs() << " move ";
            else std::cout << "info cp score " << _bestScoreIter << " depth " << depth << " time " << idTimer.getMs() << " move ";
            bestMove.print();
        }
        std::cout << "tt used:" << TTUsed << " nodesTotal:" << nodesVisited <<std::endl;
        std::cout << "tt ratio: " << (TTUsed*1.0)/nodesVisited << std::endl;
        std::cout << "pv ratio:" << (pvCounter*1.0)/nodesVisited << std::endl;
        return bestMove;
    }

    static constexpr int CHECKMATE = 1000000;
    static inline TranspositionTable* TT;

private:

    static void prepareForSearch(){
        for(int j = 0; j <= 1; j++){
            for(int d = 0; d < Board::MAX_DEPTH; d++){
                _killerMoves[d][j] = Move();
            }
        }

        for(int j = 0; j < 63; j++){
            for(int i = 0; i < 63; i++){
                _history[j][i] = 0;
            }
        }
    }

    /*
     * TODO
     * 0) SPEED-UP!
     * 4) PV + PV order.
     * 5) TT fix - DONE ???
     * 6) QSearch optimization DONE
     *      -> TT try
     */

    // https://en.wikipedia.org/wiki/Negamax with alpha beta + TT.
    static int negamax(int depth, int ply, int alpha, int beta, bool doNull, bool isPv){
        if(_timer.isTimeout()){
            _forceStopped = true;
            return 0;
        }
        assert(isPv || alpha + 1 == beta);

        nodesVisited++;

        if(_board->isDraw()) return 0;

        // Try get eval from TT.
        int ttEval = TT->getEval(depth, alpha, beta, ply);
        auto hashMove = ttEval == TranspositionTable::FOUND_NOT_ACCEPTED ? TT->getMove() : Move();

        if(ttEval > TranspositionTable::LOOKUP_ERROR && !isPv){
            TTUsed++;
            return ttEval;
        }
        // IIR
        if(ttEval == TranspositionTable::LOOKUP_ERROR && ply > 0 && depth >= 5) depth--;

        // after tt search, eval position.
        if(depth <= 0) return qsearch(alpha, beta, ply);
        bool isCheckNMP = _board->inCheck(); // If current king is checked, logically we can't do NMP (enemy will capture our king).
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

        // Razoring
        // if eval of current position is so bad, we can prune it.
        // pretty crazy and experimental.
        /*
            Without razoring - bullet
            Results of sentinel-dev vs sentinel (60+1, 1t, MB, 8moves_v3.pgn):
            Elo: -41.89 +/- 50.73, nElo: -57.20 +/- 68.10
            LOS: 4.98 %, DrawRatio: 30.00 %, PairsRatio: 0.52
            Games: 100, Wins: 26, Losses: 38, Draws: 36, Points: 44.0 (44.00 %)

            Without razoring - rapid
            Results of sentinel-dev vs sentinel (1.8e+02+1, 1t, MB, 8moves_v3.pgn):
            Elo: -147.19 +/- 148.05, nElo: -262.64 +/- 215.34
            LOS: 0.84 %, DrawRatio: 40.00 %, PairsRatio: 0.00
        */

        /*
        if(!isPv && !isCheckNMP && ply > 0 && depth <= RAZOR_MIN_DEPTH && currentEval + RAZOR_MARGIN * depth <= alpha){
            auto score = qsearch(alpha - 1, alpha);
            if(score <= alpha) return score;
        }*/

        // Futility pruning
        // If current pos - margin is too good (>= beta), we can return currentEval.
        if(!isPv && !isCheckNMP && ply > 0 && depth <= 6 && currentEval - 275 * depth >= beta){
            return currentEval;
        }

        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto [moveCount, isCheck] = Movegen::generateMoves(*_board, moves);
        std::vector<int> moveScores(moveCount);

        // "move ordering"
        Movepick::scoreMoves(moves, moveCount, *_board, _killerMoves, _history ,hashMove, moveScores);
        bool visitedAny = false;

        TranspositionTable::HashType TTType = TranspositionTable::UPPER_BOUND;
        Move bestMoveInPos;

        int movesSearched = 0; // LMR
        // bool canPrune = !isCheckNMP && depth == 1 && ply > 0; // RFT

        for(int j = 0; j < moveCount; j++){
            // pick a move to play (sorting moves, can be slower, thanks to alpha beta pruning).
            Movepick::pickMove(moves, moveCount, j, moveScores);

            // If our position is bad, quiet move is not going to make it better.
            // reverse futility pruning.
            // if(canPrune && currentEval + EFP[depth - 1] <= alpha && moves[j].score < 800) continue;

            if(!_board->makeMove(moves[j])) continue; // pseudolegal movegen.

            // late move reduction.
            int eval = lmr(depth, ply, alpha, beta, movesSearched, isPv, moveScores[j]);

            _board->undoMove(moves[j]);

            // ! after undo move !
            if(_forceStopped){
                return 0;
            }

            if(eval >= beta){
                // If move, that wasnt capture causes a beta cuttoff, we call it killer move, remember this move for move ordering.
                auto attackSquareType = _board->getPieceTypeFromSQ(moves[j].toSq, _board->whoPlay ? _board->whitePieces : _board->blackPieces);
                if(!attackSquareType.second && moves[j].promotionType == Move::NONE){
                    storeKillerMove(ply, moves[j]);
                }
                TT->store(eval, depth, TranspositionTable::LOWER_BOUND, moves[j], ply);
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

                auto attackSquareType = _board->getPieceTypeFromSQ(moves[j].toSq, _board->whoPlay ? _board->whitePieces : _board->blackPieces);
                if(!attackSquareType.second){
                    _history[moves[j].fromSq][moves[j].toSq] += depth * depth;
                }
            }


            visitedAny = true; // for draw / checkmate checking.
            movesSearched++; // for LMR
        }

        // movegen returns cnt of moves total and if king is checked.
        // if there is no move - eval == INT_MIN, we can easily return if it's draw or checkmate (for checkmate we use value < INT_MIN).
        // checkmate || draw.

        if(!visitedAny && isCheck) return -CHECKMATE + ply; // checkmate.
        if(!visitedAny && !isCheck) return 0; // draw



        TT->store(alpha, depth, TTType, bestMoveInPos, ply);
        return alpha;
    }


    /***
     * LMR - late move reduction
     * If move is quiet or not killer move, we will apply LMR
     * // https://web.archive.org/web/20150212051846/http://www.glaurungchess.com/lmr.html
     * @return
     */
    static inline int lmr(int depth, int ply, int alpha, int beta, int movesSearched, bool isPv, int moveScore) {
        int eval;

        int R = 0;
        if(depth > LMR_DEPTH && ply > 0){
            R += !isPv;
            R += moveScore == 0; // dont reduce history moves as much.
            R += movesSearched >= 4;

            R = std::clamp(R, 0, depth - 2);
        }
        if(movesSearched == 1 && isPv){
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
        return eval;
    }

    /***
     * https://www.chessprogramming.org/Quiescence_Search
     * Generates all captures possible and plays them.
     * This is used for horizon effect - If i capture with a QxP -> good move, but what if another pawn in next depth will capture my Q?!
     *  -> qsearch.
     * @return eval of the position without captures.
     */
    static int qsearch(int alpha, int beta, int ply){

        nodesVisited++;

        if(_board->isDraw()) return 0;
        if(ply >= MAX_DEPTH) return _board->eval();

        auto currentEval = _board->eval();
        if(currentEval >= beta) return beta;
        if(currentEval > alpha) alpha = currentEval;

        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto [moveCount, isCheck] = Movegen::generateMoves(*_board, moves, true);
        std::vector<int> moveScores(moveCount);
        Movepick::scoreMoves(moves, moveCount, *_board, nullptr, nullptr, Move(), moveScores);

        for(int j = 0; j < moveCount; j++){
            // pick a best move to play.
            Movepick::pickMove(moves, moveCount, j, moveScores);

            if(!_board->makeMove(moves[j])) continue; // pseudolegal movegen.
            int eval = -qsearch(-beta, -alpha, ply + 1);
            _board->undoMove(moves[j]);

            if(eval >= beta) return beta;
            if(eval > alpha) alpha = eval;
        }

        return alpha;
    }

    static inline void storeKillerMove(int ply, const Move& move){
        _killerMoves[ply][1] = std::move(_killerMoves[ply][0]);
        _killerMoves[ply][0] = std::move(move);
    }

};

#endif //SENTINEL_SEARCH_H



