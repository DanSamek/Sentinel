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
    static inline constexpr int MAX_DEPTH = 128;
    static inline constexpr int LMR_DEPTH = 3;


    static inline Board* _board;
    static inline bool _forceStopped = false;

    static inline Timer _timer;

    // Debug.
    static inline int TTUsed;
    static inline int nodesVisited;

    // Killer moves, that did beta cutoffs, use them in move order.
    // Now only 2 killer moves per ply.
    static inline Move _killerMoves[Board::MAX_DEPTH][2];

public:
    // 8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1
    static Move search(int miliseconds, Board& board, bool exact){
        TTUsed = nodesVisited = 0;
        // get first moves, only legal.
        _board = &board;
        _forceStopped = false;
        _bestScoreIter = INT_MIN;
        _bestMoveIter = {};

        int msCanBeUsed = exact ? miliseconds : miliseconds / 80; // try ??

        _timer = Timer(msCanBeUsed);
        TT->clear();
        Move bestMove;

        for(int j = 1; j <= MAX_DEPTH; j++){
            Timer idTimer;

            negamax(j, 0, NEGATIVE_INF, POSITIVE_INF, true);
            if(_forceStopped) break;

            bestMove = _bestMoveIter;
            //std::cout << "depth:" << j << " score:" << _bestScoreIter << "nodes: " << nodesVisited << " move:";
            std::cout << "info cp score " << _bestScoreIter << " depth " << j << "time " << idTimer.getMs() <<  " move ";
            bestMove.print();

            // thanks to iterative deepening + TT, we can stop search here - mate found.
            if(isMateScore(_bestScoreIter)) break;
        }
        std::cout << "tt used:" << TTUsed << " nodesTotal:" << nodesVisited <<std::endl;
        std::cout << "tt ratio: " << (TTUsed*1.0)/nodesVisited << std::endl;
        return bestMove;
    }

    static constexpr int CHECKMATE = 1000000;
    static inline TranspositionTable* TT;
private:

    /*
     * 1) killer moves (by halfmoves). DONE
     * 2) late move reduction DONE
     * 3) null move pruning
     * TODO
     * 4) PV
     * 5) TT fix
     * 6) More tests on null move pruning, now only in perft tests.
     */

    // https://en.wikipedia.org/wiki/Negamax with alpha beta + TT.
    static int negamax(int depth, int ply, int alpha, int beta, bool doNull){
        if(_timer.isTimeout()){
            _forceStopped = true;
            return 0;
        }

        nodesVisited++;

        if(_board->isDraw()) return 0;

        // Try get eval from TT.
        int ttEval = TT->getEval(depth, alpha, beta);

        if(ttEval != TranspositionTable::LOOKUP_ERROR){
            TTUsed++;
            if(ply == 0){
                _bestMoveIter = TT->getMove();
                _bestScoreIter = ttEval;
            }
            return ttEval;
        }

        // after tt search, eval position.
        if(depth <= 0) return qsearch(alpha, beta);

        // Null move pruning
        // We just give enemy next move (we dont move any piece)
        // If our position is too good, even 1 additional move for opponent cant help, we return beta.
        bool isCheckNMP = _board->inCheck(); // If current king is checked, logically we can't do NMP (enemy will capture our king).
        bool someBigPiece = _board->anyBiggerPiece(); // Zugzwang prevention, in some simple endgames can NMP hurt.

        if(depth >= 3 && doNull && !isCheckNMP && someBigPiece && ply > 0){

            _board->makeNullMove();
            int eval = -negamax(depth - 3, ply + 1, -beta, -beta + 1, false);
            _board->undoNullMove();

            if(eval >= beta) return eval;
            if(_forceStopped) return 0;
        }

        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto [moveCount, isCheck] = Movegen::generateMoves(*_board, moves);

        // "move ordering"
        Movepick::scoreMoves(moves, moveCount, *_board, _killerMoves);
        bool visitedAny = false;

        TranspositionTable::HashType TTType = TranspositionTable::UPPER_BOUND;
        Move bestMoveInPos;

        int movesSearched = 0; // LMR
        for(int j = 0; j < moveCount; j++){
            // pick a move to play (sorting moves, can be slower, thanks to alpha beta pruning).
            Movepick::pickMove(moves, moveCount, j);
            if(!_board->makeMove(moves[j])) continue; // pseudolegal movegen.

            // late move reduction.
            int eval = lmr(depth, ply, alpha, beta, movesSearched);

            _board->undoMove(moves[j]);

            // ! after undo move !
            if(_forceStopped){
                return 0;
            }

            if(eval >= beta){
                // If move, that wasnt capture causes a beta cuttoff, we call it killer move, remember this move for move ordering.
                auto attackSquareType = _board->getPieceTypeFromSQ(moves[j].toSq, _board->whoPlay ? _board->whitePieces : _board->blackPieces);
                if(!attackSquareType.second){
                    storeKillerMove(moves[j]);
                }
                TT->store(beta, depth, TranspositionTable::LOWER_BOUND, moves[j]);
                return beta;
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

        // movegen returns cnt of moves total and if king is checked.
        // if there is no move - eval == INT_MIN, we can easily return if it's draw or checkmate (for checkmate we use value < INT_MIN).
        // checkmate || draw.
        if(!visitedAny && isCheck) return -(CHECKMATE + depth); // checkmate.
        if(!visitedAny && !isCheck) return 0; // draw

        TT->store(alpha, depth, TTType, bestMoveInPos);
        return alpha;
    }


    /***
     * LMR late move reduction
     * If move is quiet or not killer move, we will apply LMR
     * Todo add pv nodes.
     * // https://web.archive.org/web/20150212051846/http://www.glaurungchess.com/lmr.html
     * @return
     */
    static inline int lmr(int depth, int ply, int alpha, int beta, int movesSearched) {
        int eval;
        if(movesSearched >= 5 && depth >= LMR_DEPTH){
            // do reduced search.
            eval = -negamax(depth - 2, ply + 1, -alpha - 1, -alpha, true);

            // if eval is bigger, than alpha, go full search
            if(eval > alpha) eval = -negamax(depth - 1, ply + 1, -alpha - 1, -alpha, true);

            // if LMR fails, do normal full search.
            if(eval > alpha && eval < beta) eval = -negamax(depth - 1, ply + 1, -beta, -alpha, true);
        }
        // normal full search.
        else{
            eval = -negamax(depth - 1, ply + 1, -beta, -alpha, true);
        }
        return eval;
    }

    /***
     * https://www.chessprogramming.org/Quiescence_Search
     * Generates all captures possible and plays them.
     * This is used for horizon effect - i capture with a QxP -> good move, but what if another pawn in next depth will capture my Q?!
     *  -> qsearch.
     * @return eval of the position without captures.
     * Maybe TODO TT (?)
     */
    static int qsearch(int alpha, int beta){
        auto currentEval = _board->eval();

        if(currentEval >= beta) return beta;
        if(currentEval > alpha) alpha = currentEval;

        Move moves[Movegen::MAX_LEGAL_MOVES];
        auto [moveCount, isCheck] = Movegen::generateMoves(*_board, moves, true);
        Movepick::scoreMoves(moves, moveCount, *_board, nullptr);

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

    static inline void storeKillerMove(const Move& move){
        _killerMoves[_board->halfMove][1] = std::move(_killerMoves[_board->halfMove][0]);
        _killerMoves[_board->halfMove][0] = std::move(move);
    }

};

#endif //SENTINEL_SEARCH_H



