#include "movepick.h"

void Movepick::scoreMoves(Move *moves, int cnt, Board &board, const Move killerMoves[Board::MAX_DEPTH][2], const Move& hashMove) {
    for(int j = 0; j < cnt; j++){
        int score = 0;
        auto move = moves[j];

        if(move.moveType == Move::CAPTURE){
            auto attacker = move.movePiece;
            auto victim = board.getPieceType(move.toSq);
            score += MVV_VLA[attacker][victim];
        }

        if(killerMoves != nullptr) {
            for(int j = 0; j < 2; j++){
                if(move == killerMoves[board.halfMove][j]){
                    score += KILLER_MOVES_ORDER_SCORE[j];
                }
            }
        }

        if(hashMove == move){
            score += TT_MOVE_ORDER_SCORE;
        }

        // promotion is good.
        if(move.promotionType != 0){
            score += move.promotionType * 7;
        }

        moves[j].score = score;
    }
}

void Movepick::pickMove(Move *moves, int cnt, int startingIndex) {
    for(int j = startingIndex + 1; j < cnt; j++){
        if(moves[j].score > moves[startingIndex].score){
            std::swap(moves[j], moves[startingIndex]);
        }
    }
}