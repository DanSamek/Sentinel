#ifndef SENTINEL_STATE_H
#define SENTINEL_STATE_H
#include "move.h"
#include <array>
#include <cstring>


struct State{
    State(int captureType, int enPassantSquare, const std::array<std::array<bool, 2>, 2> &castling, int halfMove,
          uint64_t zobristHash, int fullMove, uint64_t _whitePieces[6], uint64_t _blackPieces[6])
          : captureType(captureType), enPassantSquare(enPassantSquare),castling(castling), halfMove(halfMove),
          zobristHash(zobristHash), fullMove(fullMove){

        memcpy(whitePieces, _whitePieces, 6* sizeof (uint64_t));
        memcpy(blackPieces, _blackPieces, 6* sizeof (uint64_t));
    }
    State() = default;

    int captureType = -1;
    int enPassantSquare = -1;
    std::array<std::array<bool, 2>, 2> castling;
    int halfMove;
    uint64_t zobristHash;
    int fullMove;
    uint64_t whitePieces[6];
    uint64_t blackPieces[6];
};

#endif //SENTINEL_STATE_H
