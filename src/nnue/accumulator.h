#ifndef SENTINEL_ACCUMULATOR_H
#define SENTINEL_ACCUMULATOR_H

#include "array"
#include "const.h"
#include "board.h"

// Part of an UE [effective updates]
// After each move we can update accumulator ["pre-calculation of IL->HL"].
    // We can observe, that we just + / - weight to HL.
// This is much faster, than full forward via net.
// Calculations are from HL -> OL.
class Accumulator {
    // 0 -> white.
    // 1 -> black.
    std::array<std::array<int, HIDDEN_LAYER_SIZE>, 2> data;

public:
    /***
     * Updates accumulator for a color - add: move from -> to, initialization of a pieces.
     */
    template<Board::pieceColor color>
    void add(const std::array<int, HIDDEN_LAYER_SIZE>& weights) {
        for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
            data[color][i] += weights[i];
        }
    }

    /***
     * Updates accumulator for a color - substitution -> capture/ move from -> to.
     */
    template<Board::pieceColor color>
    void sub(const std::array<int, HIDDEN_LAYER_SIZE>& weights) {
        for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
            data[color][i] -= weights[i];
        }
    }

    template<Board::pieceColor color>
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
