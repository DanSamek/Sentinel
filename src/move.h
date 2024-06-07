#ifndef SENTINEL_MOVE_H
#define SENTINEL_MOVE_H

struct Move {
    enum type{
        QUIET,
        CAPTURE,
        EN_PASSANT,
        CASTLING
    };
    int from_sq;
    int to_sq;
};

#endif //SENTINEL_MOVE_H
