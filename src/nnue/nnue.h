#ifndef SENTINEL_NNUE_H
#define SENTINEL_NNUE_H

#include "array"
#include "const.h"
#include "string"
#include "accumulator.h"
#include "algorithm"

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

public:
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
     * stackIndex = 0.
     */
    void reset();

    /***
     * see documentation of a accumulator.add()
     */
    void updateAccumulatorSub(Board::pieceColor color, Board::pieceType piece, int square);

    /***
     * see documentation of a accumulator.add()
     */
    void updateAccumulatorAdd(Board::pieceColor color, Board::pieceType piece, int square);

    /***
     * @return eval of a current "position".
     */
    template<Board::pieceColor perspective>
    int eval();


    template<Board::pieceColor perspective>
    inline int getIndex(Board::pieceColor color, Board::pieceType piece, int square) {
        auto colorIndex = (perspective == color) ? 0 : 1;
        auto pieceIndex = (int)piece;
        auto squareIndex = (perspective == Board::WHITE) ? square ^ 56 : square; // ?

        auto result = (colorIndex * 6 + pieceIndex) * 64 + squareIndex;
        return result;
    }
};


#endif //SENTINEL_NNUE_H
