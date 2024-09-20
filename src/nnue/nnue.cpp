#include "nnue.h"
#include "fstream"
#include "cassert"

template<typename T>
T readNumber(std::ifstream& stream){
    T value;
    stream.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
}

NNUE::NNUE() {
    reset();
    load();
}

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

void NNUE::updateAccumulatorAdd(PIECE_COLOR color, PIECE_TYPE piece, int square) {
    auto indexWhite = getIndex<PIECE_COLOR::WHITE>(color, piece, square);
    auto indexBlack = getIndex<PIECE_COLOR::BLACK>(color, piece, square);

    auto accumulator = &stack[stackIndex];

    accumulator->add<PIECE_COLOR::WHITE>(INPUT_LAYER[indexWhite]);
    accumulator->add<PIECE_COLOR::BLACK>(INPUT_LAYER[indexBlack]);
}

void NNUE::updateAccumulatorSub(PIECE_COLOR color, PIECE_TYPE piece, int square) {
    auto indexWhite = getIndex<PIECE_COLOR::WHITE>(color, piece, square);
    auto indexBlack = getIndex<PIECE_COLOR::BLACK>(color, piece, square);

    auto accumulator = &stack[stackIndex];

    accumulator->sub<PIECE_COLOR::WHITE>(INPUT_LAYER[indexWhite]);
    accumulator->sub<PIECE_COLOR::BLACK>(INPUT_LAYER[indexBlack]);
}


void NNUE::load() {
    std::ifstream stream(NET_PATH, std::ios::binary);

    if(!stream.is_open()){
        throw "File cannot be opened";
    }

    for(int i = 0; i < INPUT_LAYER_SIZE; i++){
        for(int x = 0; x < HIDDEN_LAYER_SIZE; x++){
            INPUT_LAYER[i][x] = readNumber<int16_t>(stream);
        }
    }
    assert(!stream.eof());
    for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
        INPUT_LAYER_BIASES[i] = readNumber<int16_t>(stream);
    }

    assert(!stream.eof());
    for(int i = 0; i < HIDDEN_LAYER_SIZE * 2; i++){
        HIDDEN_LAYER_WEIGHTS[i] = readNumber<int16_t>(stream);
    }
    assert(!stream.eof());

    HIDDEN_LAYER_BIASES[0] = readNumber<int16_t>(stream);
}

void NNUE::moveAPiece(PIECE_COLOR color, PIECE_TYPE piece, int fromSquare, int toSquare) {
    updateAccumulatorSub(color, piece, fromSquare); // -> 0
    updateAccumulatorAdd(color, piece, toSquare); // -> 1
}