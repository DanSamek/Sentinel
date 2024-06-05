#include "bitboard.h"

bool Bitboard::getNthBit(int pos) const{
    return value & (1ULL << pos);
}

void Bitboard::setNthBit(int pos){
    value |= (1ULL << pos);
}

void Bitboard::popNthBit(int pos) {
    if(getNthBit(pos)) value ^= (1ULL << pos);
}

std::array<std::array<bool, 8>, 8> Bitboard::generateBoardFromBitboard(){
    std::array<std::array<bool, 8>, 8> result;
    for(int rank = 0; rank < 8; rank++ ) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            result[rank][file] = getNthBit(square);
        }
    }
    return result;
}

Bitboard Bitboard::operator&(const Bitboard &bitboard) {
    auto val = value & bitboard.value;
    return Bitboard(val, type, color);
}

Bitboard Bitboard::operator|(const Bitboard &bitboard) {
    auto val = value | bitboard.value;
    return Bitboard(val, type, color);
}

Bitboard Bitboard::operator^(const Bitboard &bitboard) {
    auto val = value ^ bitboard.value;
    return Bitboard(val, type, color);
}
