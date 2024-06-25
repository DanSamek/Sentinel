#ifndef SENTINEL_MOVEPICK_H
#define SENTINEL_MOVEPICK_H

#include <move.h>
#include <board.h>

class Movepick {

    // attacker -> victim
    /*
      PAWN,
      KNIGHT,
      BISHOP,
      ROOK,
      QUEEN,
      KING
     */
    // MVV-LVA (most valuable victim, least valuable attacker).
    static constexpr int MVV_VLA[6][6] ={
        {20, 22, 24, 27, 30, 0},
        {19, 21, 23, 26, 29, 0},
        {18, 20, 22, 25, 28, 0},
        {17, 19, 21, 24, 27, 0},
        {16, 18, 20, 23, 26, 0},
        {15, 17, 19, 22, 25, 0},
    };
public:

    static void scoreMoves(Move* moves, int cnt, Board& board){
        for(int j = 0; j < cnt; j++){
            int score = 0;
            auto move = moves[j];

            if(move.moveType == Move::CAPTURE){
                auto attacker = move.movePiece;
                auto victim = board.getPieceType(move.toSq);
                score += MVV_VLA[attacker][victim];
            }

            // promotion is good.
            if(move.promotionType != 0){
                score += move.promotionType * 7;
            }

            moves[j].score = score;
        }
    }

    static void pickMove(Move* moves, int cnt, int startingIndex){
        for(int j = startingIndex + 1; j < cnt; j++){
            // swap moves.
            if(moves[j].score > moves[startingIndex].score){
                std::swap(moves[j], moves[startingIndex]);
            }
        }
    }
};


#endif //SENTINEL_MOVEPICK_H
