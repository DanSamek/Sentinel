#include "nnue.h"

void NNUE::push() {
    stack[stackIndex + 1] = stack[stackIndex];
    stackIndex++;
}

void NNUE::pop() {
    stackIndex--;
}

void NNUE::reset() {
    stack[0].init(INPUT_LAYER_BIASES);
    stackIndex = 0;
}

void NNUE::updateAccumulatorAdd(Board::pieceColor color, Board::pieceType piece, int square) {
    auto indexWhite = getIndex<Board::WHITE>(color, piece, square);
    auto indexBlack = getIndex<Board::BLACK>(color, piece, square);

    auto accumulator = &stack[stackIndex];

    accumulator->add<Board::WHITE>(INPUT_LAYER[indexWhite]);
    accumulator->add<Board::BLACK>(INPUT_LAYER[indexBlack]);
}

void NNUE::updateAccumulatorSub(Board::pieceColor color, Board::pieceType piece, int square) {
    auto indexWhite = getIndex<Board::WHITE>(color, piece, square);
    auto indexBlack = getIndex<Board::BLACK>(color, piece, square);

    auto accumulator = &stack[stackIndex];

    accumulator->sub<Board::WHITE>(INPUT_LAYER[indexWhite]);
    accumulator->sub<Board::BLACK>(INPUT_LAYER[indexBlack]);
}

template<Board::pieceColor perspective>
int NNUE::eval() {
    int result = HIDDEN_LAYER_BIASES[0];
    auto accumulator = &stack[stackIndex];

    for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
        result += HIDDEN_LAYER_WEIGHTS[i] * crelu(accumulator->get<perspective>()[i]);
    }

    for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
        result += HIDDEN_LAYER_WEIGHTS[i + HIDDEN_LAYER_SIZE] * crelu(accumulator->get<!perspective>()[i]);
    }

    result *= SCALE;
    result /= QA * QB;
    return result;
}