#ifndef SENTINEL_MOVE_H
#define SENTINEL_MOVE_H

#include <iostream>
#include <string>

struct Move {
    enum type{
        CAPTURE,
        PROMOTION,
        QUIET,
        EN_PASSANT,
        CASTLING,
        DOUBLE_PAWN_UP
    };
    enum PromotionType{
        NONE,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
    };
    int fromSq = 0;
    int toSq = 0;
    PromotionType promotionType = NONE;
    type moveType = QUIET;
    int movePiece;


    // Move score for movepick in search.
    int score = 0;

    /***
     * Prints a move for UCI communication
     */
    void print();

    /***
     * @param index Bitboard bit position/index (square pos)
     * @return
     */
    static std::string indexToChessSquare(int index);
};

#endif //SENTINEL_MOVE_H
