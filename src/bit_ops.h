
#ifndef SENTINEL_BIT_OPS_H
#define SENTINEL_BIT_OPS_H

#include <cstdint>
#include <array>

struct bit_ops {
    inline static bool getNthBit(const uint64_t& value, int pos){
        return value & (1ULL << pos);
    }

    inline static void setNthBit(uint64_t & value, int pos){
        value |= (1ULL << pos);
    }

    inline static void popNthBit(uint64_t & value ,int pos){
        if(getNthBit(value, pos)) value ^= (1ULL << pos);
    }

    inline static int bitScanForward(const uint64_t & value){
        return __builtin_ctzll(value); // lsb
    }

    inline static int bitScanForwardPopLsb(uint64_t& value){
        auto result = bitScanForward(value);
        value &= value - 1;
        return result;
    }

    static std::array<std::array<bool, 8>, 8> generateBoardFromBitboard(const uint64_t& bb) {
        std::array<std::array<bool, 8>, 8> result;
        for(int rank = 0; rank < 8; rank++ ) {
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                result[rank][file] = getNthBit(bb, square);
            }
        }
        return result;
    }
};


#endif //SENTINEL_BIT_OPS_H
