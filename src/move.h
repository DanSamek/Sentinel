#ifndef SENTINEL_MOVE_H
#define SENTINEL_MOVE_H

#include <iostream>
#include <string>

struct Move {
    enum type :int8_t{
        CAPTURE,
        PROMOTION,
        QUIET,
        EN_PASSANT,
        CASTLING,
        DOUBLE_PAWN_UP
    };
    enum PromotionType : int8_t{
        NONE,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
    };
    int8_t fromSq = 0;
    int8_t toSq = 0;
    PromotionType promotionType = NONE;
    type moveType = QUIET;
    int8_t movePiece;

    /***
     * Prints a move for UCI communication
     */
    void print();

    /***
     * @param index Bitboard bit position/index (square pos)
     * @return
     */
    static std::string indexToChessSquare(int index);

    /***
     * Move comparer
     * @return if moves are same.
     */
    bool operator ==(const Move& other);

    /***
     * Default .ctor
     */
    Move() : fromSq(-1) {};

    Move(int fromSq, int toSq, PromotionType promotionType, type moveType, int movePiece) : fromSq(fromSq), toSq(toSq), promotionType(promotionType), moveType(moveType),movePiece(movePiece) {}
};

#endif //SENTINEL_MOVE_H
