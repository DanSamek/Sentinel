#include "movepick.h"

void Movepick::scoreMoves(Move *moves, int cnt, Board &board, const Move killerMoves[Board::MAX_DEPTH][2], int history[64][64],const Move& hashMove, std::vector<int>& moveScores) {
    for(int moveIndex = 0; moveIndex < cnt; moveIndex++){
        int score = 0;
        auto move = moves[moveIndex];
        auto isCapture = move.isCapture();

        // MVV_VLA
        if(isCapture){
            auto attacker = move.movePiece;
            auto victim = move.moveType == Move::EN_PASSANT ? Board::PAWN : board.getPieceType(move.toSq);
            score += MVV_VLA[attacker][victim];
        }

        // tt move
        if(hashMove == move){
            score += TT_MOVE_ORDER_SCORE;
        }

        // promotion is good.
        if(move.isPromotion()){
            score += move.promotionType * 70000;
        }

        // history score.
        if(!isCapture){
            // killers
            for(int killerIndex = 0; killerIndex < 2; killerIndex++){
                if(move == killerMoves[board.halfMove][killerIndex]){
                    score += KILLER_MOVES_ORDER_SCORE[killerIndex];
                    break;
                }
            }
            score += history[move.fromSq][move.toSq];
        }

        moveScores[moveIndex] = score;
    }
}

void Movepick::scoreMovesQSearch(Move *moves, int cnt, Board &board, const Move &hashMove, std::vector<int> &moveScores) {
    for(int moveIndex = 0; moveIndex < cnt; moveIndex++){
        int score = 0;
        auto move = moves[moveIndex];

        auto attacker = move.movePiece;
        auto victim = move.moveType == Move::EN_PASSANT ? Board::PAWN : board.getPieceType(move.toSq);
        score += MVV_VLA[attacker][victim];

        if(hashMove == move){
            score += TT_MOVE_ORDER_SCORE;
        }

        // promotion capture is good.
        if(move.isPromotion()){
            score += move.promotionType * 7;
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