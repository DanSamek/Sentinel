#include "bit_ops.h"

bool bit_ops::getNthBit(const uint64_t& value, int pos){
    return value & (1ULL << pos);
}

void bit_ops::setNthBit(uint64_t & value, int pos){
    value |= (1ULL << pos);
}

void bit_ops::popNthBit(uint64_t & value ,int pos) {
    if(getNthBit(value, pos)) value ^= (1ULL << pos);
}