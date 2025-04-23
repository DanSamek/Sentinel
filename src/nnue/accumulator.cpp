#include "accumulator.h"

namespace Sentinel{
    void Accumulator::init(const ::std::array<int, HIDDEN_LAYER_SIZE> &biases) {
        data[0] = biases;
        data[1] = biases;
    }
}