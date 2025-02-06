#ifndef SENTINEL_NEURALNETWORK_H
#define SENTINEL_NEURALNETWORK_H

#include "array"
#include "const.h"
#include "string"
#include "board.h"
#include "algorithm"

// Non optimized neural network.
// Experimenting before adding Accumulators, SIMD, ...
// And verification of eval, if NNUE works same.
    // Same == both implementations can be wrong :D
class NeuralNetwork {
    std::array<std::array<int, HIDDEN_LAYER_SIZE>, INPUT_LAYER_SIZE> INPUT_LAYER;
    std::array<int, HIDDEN_LAYER_SIZE> INPUT_LAYER_BIASES;
    std::array<int, HIDDEN_LAYER_SIZE * 2> HIDDEN_LAYER_WEIGHTS;
    std::array<int, OUTPUT_SIZE> HIDDEN_LAYER_BIASES;

    static constexpr int QA = 255;
    static constexpr int QB = 64;
    static constexpr int SCALE = 400;

    static inline std::string NET_PATH = "../src/nnue/alpha.bin";
public:
    /***
     * Loads a NN from a binary file.
     */
    void load();

    /***
     * Simple pass of a NN.
     * @return
     */
    int eval(const Board& board);

    /***
     * Simple speedtest.
     */
    void test();

private:
    std::array<int, HIDDEN_LAYER_SIZE> forwardInputLayer(const std::vector<int>& inputLayer);

    inline int crelu(int value) {
        return std::clamp(value, 0, QA);
    }
};


#endif //SENTINEL_NEURALNETWORK_H
