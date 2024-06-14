
#ifndef SENTINEL_BIT_OPS_H
#define SENTINEL_BIT_OPS_H


#include <cstdint>

struct bit_ops {
    static bool getNthBit(const uint64_t& value, int pos);
    static void setNthBit(uint64_t & value, int pos);
    static void popNthBit(uint64_t & value ,int pos);
    static int bitScanForward(const uint64_t & value);
    static int bitScanForwardPopLsb(uint64_t& value);
};


#endif //SENTINEL_BIT_OPS_H
