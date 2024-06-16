#ifndef SENTINEL_MOVE_H
#define SENTINEL_MOVE_H

struct Move {
    enum type{
        CAPTURE,
        PROMOTION,
        QUIET,
        EN_PASSANT,
        CASTLING,
        DOUBLE_PAWN_UP
    };
    enum promotionType{
        NONE,
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT
    };
    int fromSq = 0;
    int toSq = 0;
    promotionType promotionType = NONE;
    type moveType = QUIET;
    int movePiece;
};

#endif //SENTINEL_MOVE_H
