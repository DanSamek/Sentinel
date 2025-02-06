#ifndef SENTINEL_BOARDENUMS_H
#define SENTINEL_BOARDENUMS_H

#include <cstdint>

enum PIECE_TYPE : int8_t{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum PIECE_COLOR : int8_t{
    WHITE,
    BLACK
};


#endif //SENTINEL_BOARDENUMS_H
