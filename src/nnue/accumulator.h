#ifndef SENTINEL_ACCUMULATOR_H
#define SENTINEL_ACCUMULATOR_H

#ifdef __AVX__
#include <immintrin.h>
#endif

#include "array"
#include "const.h"
#include "boardEnums.h"

// Part of an UE [effective updates]
// After each move we can update accumulator ["pre-calculation of IL->HL"].
    // We can observe, that we just + / - weight to HL.
// This is much faster, than full forward via net.
// Calculations are from HL -> OL.
class Accumulator {
    // 0 -> white.
    // 1 -> black.
    alignas(32) std::array<std::array<int, HIDDEN_LAYER_SIZE>, 2> data = {};

public:
    /***
     * Updates accumulator for a color - add: move from -> to, initialization of a pieces.
     */
    template<PIECE_COLOR color>
    void add(const std::array<int, HIDDEN_LAYER_SIZE>& weights) {
        #ifdef __AVX__
            for (int i = 0; i < HIDDEN_LAYER_SIZE; i += 8) {
                auto vec_weights = _mm256_load_si256((const __m256i *)&weights[i]);
                auto accumulator_data = _mm256_load_si256((const __m256i*)&data[color][i]);
                auto result = _mm256_add_epi32(vec_weights, accumulator_data);
                _mm256_store_si256((__m256i *)&data[color][i], result);
            }
        #else
                for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
                    data[color][i] += weights[i];
                }
        #endif
    }

    /***
     * Updates accumulator for a color - substitution -> capture/ move from -> to.
     */
    template<PIECE_COLOR color>
    void sub(const std::array<int, HIDDEN_LAYER_SIZE>& weights) {
        #ifdef __AVX__
            for (int i = 0; i < HIDDEN_LAYER_SIZE; i += 8) {
                auto vec_weights = _mm256_load_si256((const __m256i *)&weights[i]);
                auto accumulator_data = _mm256_load_si256((const __m256i*)&data[color][i]);
                auto result = _mm256_sub_epi32(accumulator_data, vec_weights);
                _mm256_store_si256((__m256i *)&data[color][i], result);
            }
        #else
            for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
                data[color][i] -= weights[i];
            }
        #endif
    }

    template<PIECE_COLOR color>
    const std::array<int, HIDDEN_LAYER_SIZE> &get() {
        return data[color];
    }

    /***
     * Initialization of an accumulator.
     * -> Only biases, entire board [pieces] will be added using add.
     * @param biases L1 biases
     */
    void init(const std::array<int, HIDDEN_LAYER_SIZE>& biases);
};


#endif //SENTINEL_ACCUMULATOR_H
