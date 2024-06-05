#ifndef SENTINEL_BITBOARD_H
#define SENTINEL_BITBOARD_H

#include <cstdint>
#include <iostream>
#include <array>

/***
 * Bitboard struct - only for "easier" work with bitboards.
 * Represented from top:
 * black
 * 1 2 4 8 16 32 64 128
 * 0 0 0 0 ...
 * white
 * ....
 * ! board is
 * ..
 * 4
 * 3
 * 2
 * 1
 *   a b c d e f g h
 */
struct Bitboard {
    /***
     * Types of bitboards - for each piece.
     */
    enum pieceType{
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };
    /***
     * Piece colors
     */
    enum pieceColor{
        BLACK,
        WHITE
    };

    uint64_t value = 0ULL;
    pieceType type;
    pieceColor color;

    Bitboard(uint64_t _value, pieceType _type, pieceColor _pieceColor) : value(_value), type(_type), color(_pieceColor) {};
    Bitboard(uint64_t _value) : value(_value) {};
    Bitboard() = default;
    // Debug only.
    void printBoard(){
        for(int rank = 0; rank < 8; rank++ ){
            for(int file = 0; file < 8; file++){
                int square = rank * 8 + file;
                std::cout << ( getNthBit(square) ? 1 : 0  ) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    /***
     * @param pos position
     * @note: position is +1 -> as in array or number first bit = 0
     */
    bool getNthBit(int pos) const;

    /***
     * @param pos position
     * @note: position is +1 -> as in array or number first bit = 0
     */
    void setNthBit(int pos);

    /***
     * @param pos position
     * Removes nth bit, only if its set!
     * @note: position is +1 -> as in array or number first bit = 0
     */
    void popNthBit(int pos);

    /***
     * Function, that converts bitboard to 2d board
     */
    std::array<std::array<bool, 8>, 8> generateBoardFromBitboard();


    Bitboard operator & (const Bitboard& bitboard);
    Bitboard operator | (const Bitboard& bitboard);
    Bitboard operator ^ (const Bitboard& bitboard);
};


#endif //SENTINEL_BITBOARD_H
