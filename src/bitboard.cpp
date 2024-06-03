#include "bitboard.h"

bool Bitboard::getNthBit(int pos) {
    return value & (1ULL << pos);
}

void Bitboard::setNthBit(int pos){
    value |= (1ULL << pos);
}

void Bitboard::popNthBit(int pos) {
    if(getNthBit(pos)) value ^= (1ULL << pos);
}