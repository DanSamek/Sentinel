#ifndef SENTINEL_NEURALNETWORK_H
#define SENTINEL_NEURALNETWORK_H

#include "array"
#include "const.h"
#include "string"
#include "board.h"

// Non optimized neural network.
// Experimenting before adding Accumulators, SIMD, ...
class NeuralNetwork {
    std::array<std::array<int, HIDDEN_LAYER_SIZE>, INPUT_LAYER_SIZE> INPUT_LAYER;
    std::array<int, HIDDEN_LAYER_SIZE> INPUT_LAYER_BIASES;
    std::array<int, HIDDEN_LAYER_SIZE * 2> HIDDEN_LAYER_WEIGHTS;
    std::array<int, OUTPUT_SIZE> HIDDEN_LAYER_BIASES;

    const int QA = 255;
    const int QB = 64;
    const int SCALE = 400;

    const std::string NET_PATH = "/home/daniel/Desktop/Sentinel/src/nnue/hl16.bin";
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

private:
    // TODO create 1 method for forwardInputLayer & forwardHiddenLayer.
    std::array<int, HIDDEN_LAYER_SIZE> forwardInputLayer(const std::array<int, INPUT_LAYER_SIZE>& inputLayer);

    // TODO create 1 method for forwardInputLayer & forwardHiddenLayer.
    int  forwardHiddenLayer(const std::array<int, HIDDEN_LAYER_SIZE * 2>& hiddenLayer);

    int crelu(int value);
};


#endif //SENTINEL_NEURALNETWORK_H
