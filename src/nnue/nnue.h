#ifndef SENTINEL_NNUE_H
#define SENTINEL_NNUE_H

#include <cassert>
#include "array"
#include "const.h"
#include "string"
#include "accumulator.h"
#include "algorithm"
#include "iostream"

/***
 * Neural network with effective updates
 * -> Accumulators.
 */
class NNUE {
    std::array<std::array<int, HIDDEN_LAYER_SIZE>, INPUT_LAYER_SIZE> INPUT_LAYER;
    std::array<int, HIDDEN_LAYER_SIZE> INPUT_LAYER_BIASES;

    std::array<int, HIDDEN_LAYER_SIZE * 2> HIDDEN_LAYER_WEIGHTS;
    std::array<int, OUTPUT_SIZE> HIDDEN_LAYER_BIASES;

    static constexpr int QA = 255;
    static constexpr int QB = 64;
    static constexpr int SCALE = 400;

    static inline std::string NET_PATH = "../src/nnue/alpha.bin";

    std::array<Accumulator, 1024> stack;
    int stackIndex = 0;

    /***
     * Activation function of a HL.
     */
    inline int crelu(int value) {
        return std::clamp(value, 0, QA);
    }

    void load();

public:
    NNUE();
    /***
     * Saves current accumulator to a stack.
     * For easier board.undoMove();
     */
    void push();

    /***
     * Restores previous accumulator from a stack.
     * For easier board.undoMove();
     */
    void pop();

    /***
     * StackIndex = 0.
     */
    void reset();

    /***
     * See documentation of a accumulator.add()
     */
    void updateAccumulatorSub(PIECE_COLOR color, PIECE_TYPE piece, int square);

    /***
     * See documentation of a accumulator.add()
     */
    void updateAccumulatorAdd(PIECE_COLOR color, PIECE_TYPE piece, int square);

    /***
     * Updates accumulators for a piece [from -> to]
     */
    void moveAPiece(PIECE_COLOR color, PIECE_TYPE piece, int fromSquare, int toSquare);

    /***
     * @return eval of a current "position".
     */
    template<PIECE_COLOR perspective>
    int eval(){
        constexpr auto opp = perspective == WHITE ? BLACK : WHITE;
        int result = HIDDEN_LAYER_BIASES[0];
        auto accumulator = &stack[stackIndex];

        auto ourAccumulator = accumulator->get<perspective>();
        for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
            result += HIDDEN_LAYER_WEIGHTS[i] * crelu(ourAccumulator[i]);
        }

        auto oppAccumulator = accumulator->get<opp>();
        for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
            result += HIDDEN_LAYER_WEIGHTS[i + HIDDEN_LAYER_SIZE] * crelu(oppAccumulator[i]);
        }

        result *= SCALE;
        result /= QA * QB;
        return result;
    }

    template<PIECE_COLOR perspective>
    inline int getIndex(PIECE_COLOR color, PIECE_TYPE piece, int square) {
        auto colorIndex = (perspective == color) ? 0 : 1;
        auto pieceIndex = (int)piece;
        auto squareIndex = (perspective == PIECE_COLOR::WHITE) ? square ^ 56 : square;

        auto result = (colorIndex * 6 + pieceIndex) * 64 + squareIndex;
        assert(result >= 0 && result < 768);
        return result;
    }
};


#endif //SENTINEL_NNUE_H
