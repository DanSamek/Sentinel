#ifndef SENTINEL_NNUE_H
#define SENTINEL_NNUE_H

#include <cassert>
#include "array"
#include "const.h"
#include "string"
#include "accumulator.h"
#include "algorithm"
#include "iostream"
#include "simd.h"

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


    std::array<Accumulator, 1024> stack;
    int stackIndex = 0;

    /***
     * Activation function of a HL.
     */
    inline int crelu(int value) {
        return std::clamp(value, 0, QA);
    }

#if USE_SIMD
    inline simd_type avx_crelu(const simd_type& value){
        const auto zero = SIMD_SET1(0);
        const auto qa = SIMD_SET1(QA);
        auto clamped = SIMD_MIN(SIMD_MAX(value, zero), qa);
        return clamped;
    }
#endif

    void load();

    void loadFromFile();
    void loadInlined();

    void print();

public:
    static inline bool inlineNet = true;
    static inline std::string NET_PATH = "./src/nnue/singularity_v2_4-40.bin";
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

        auto accumulator = &stack[stackIndex];
        int result = HIDDEN_LAYER_BIASES[0];
        auto ourAccumulator = accumulator->get<perspective>();
        auto oppAccumulator = accumulator->get<opp>();

    #if USE_SIMD
        alignas(32) std::array<int, simd_jmp> array_result = {0};
        auto vector_result = SIMD_LOAD((simd_type*)&array_result);
        for(int i = 0; i < HIDDEN_LAYER_SIZE; i += simd_jmp){
            auto weights = SIMD_LOAD((const simd_type*)&HIDDEN_LAYER_WEIGHTS[i]);
            auto accumulator_data = avx_crelu(SIMD_LOAD((const simd_type*)&ourAccumulator[i]));
            auto mul = SIMD_MUL(weights, accumulator_data);
            vector_result = SIMD_ADD(mul, vector_result);
        }

        for(int i = 0; i < HIDDEN_LAYER_SIZE; i += simd_jmp){
            auto weights = SIMD_LOAD((const simd_type*)&HIDDEN_LAYER_WEIGHTS[i+HIDDEN_LAYER_SIZE]);
            auto accumulator_data = avx_crelu(SIMD_LOAD((const simd_type*)&oppAccumulator[i]));
            auto mul = SIMD_MUL(weights, accumulator_data);
            vector_result = SIMD_ADD(mul, vector_result);
        }
        SIMD_STORE((simd_type*)&array_result, vector_result);
        for(int i = 0; i < simd_jmp; i++){
            result += array_result[i];
        }
    #else
        for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
            result += HIDDEN_LAYER_WEIGHTS[i] * crelu(ourAccumulator[i]);
        }
        for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
            result += HIDDEN_LAYER_WEIGHTS[i + HIDDEN_LAYER_SIZE] * crelu(oppAccumulator[i]);
        }
    #endif

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
