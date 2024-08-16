#ifndef SENTINEL_STATE_H
#define SENTINEL_STATE_H
#include "move.h"
#include <array>

struct State{
    int captureType = -1; // can be capture
    int enPassantSquare = -1;
    std::array<std::array<bool, 2>, 2> castling; // current state castling.
    std::array<int, 2> fiftyMoveRule;
    uint64_t zobristHash;
};

#endif //SENTINEL_STATE_H
