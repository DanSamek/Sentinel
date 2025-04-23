#include "movepick.h"

namespace Sentinel{
    void Movepick::scoreMoves(Move *moves, int cnt, Board &board, const History& moveHistory,
                              const Move& hashMove, const Move& counterMove, std::vector<int>& moveScores) {
        for(int moveIndex = 0; moveIndex < cnt; moveIndex++){
            int score = 0;
            auto move = moves[moveIndex];
            auto isCapture = move.isCapture();

            // MVV_VLA
            if(isCapture){
                auto attacker = move.movePiece;
                auto victim = move.moveType == Move::EN_PASSANT ? PIECE_TYPE::PAWN : board.getPieceType(move.toSq);
                score = MVV_VLA[attacker][victim];
            }
            else{
                // history score.
                score = moveHistory.history[move.fromSq][move.toSq];

                // countermoves
                if(move == counterMove){
                    score = COUNTER_MOVE_SCORE;
                }

                // killers
                for(int killerIndex = 0; killerIndex < 2; killerIndex++){
                    if(move == moveHistory.killerMoves[board.ply][killerIndex]){
                        score = KILLER_MOVES_ORDER_SCORE[killerIndex];
                        break;
                    }
                }
            }

            // promotion is good.
            if(move.isPromotion()){
                score += move.promotionType * 700;
            }

            // tt move
            if(hashMove == move){
                score = TT_MOVE_ORDER_SCORE;
            }

            moveScores[moveIndex] = score;
        }
    }

    void Movepick::scoreMovesQSearch(Move *moves, int cnt, Board &board, const Move &hashMove, std::vector<int> &moveScores) {
        for(int moveIndex = 0; moveIndex < cnt; moveIndex++){
            int score = 0;
            auto move = moves[moveIndex];

            auto attacker = move.movePiece;
            auto victim = move.moveType == Move::EN_PASSANT ? PIECE_TYPE::PAWN : board.getPieceType(move.toSq);
            score += MVV_VLA[attacker][victim];

            if(hashMove == move){
                score += TT_MOVE_ORDER_SCORE;
            }

            // promotion capture is good.
            if(move.isPromotion()){
                score += move.promotionType * 700;
            }

            moveScores[moveIndex] = score;
        }
    }

    void Movepick::pickMove(Move *moves, int cnt, int startingIndex, std::vector<int>& moveScores) {
        for(int j = startingIndex + 1; j < cnt; j++){
            if(moveScores[j] > moveScores[startingIndex]){
                std::swap(moves[j], moves[startingIndex]);
                std::swap(moveScores[j], moveScores[startingIndex]);
            }
        }
    }

}