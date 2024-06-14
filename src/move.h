#ifndef SENTINEL_MOVE_H
#define SENTINEL_MOVE_H

#include "board.h"

struct Move {
    enum type{
        CAPTURE,
        CHECK,
        PROMOTION,
        QUIET,
        EN_PASSANT,
        CASTLING
    };
    enum promotionType{
        NONE,
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT
    };
    int fromSq;
    int toSq;
    promotionType promotionType = NONE;
    type moveType = QUIET;
    Board::pieceType movePiece;
};

#endif //SENTINEL_MOVE_H
