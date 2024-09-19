#include "neuralnetwork.h"
#include "fstream"
#include "algorithm"


template<typename T>
T readNumber(std::ifstream& stream){
    T value;
    stream.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
}

int getIndex(bool us, Board::pieceType piece, int square){
    auto index = us ? (int)piece * 64 + square : ((int)piece + 6) * 64 + square;
    return index;
}

template<Board::pieceColor us, Board::pieceColor opp>
std::array<int, INPUT_LAYER_SIZE> getInputLayer(const Board& board){
    std::array<int, INPUT_LAYER_SIZE> result = {0};

    for(int square = 0; square< 63; square++){
        auto piece = board.getPieceTypeFromSQ(square, board.colorBBS(us));
        if(piece.second){
            result[getIndex(true, piece.first, square)] = 1;
            continue;
        }
        piece = board.getPieceTypeFromSQ(square, board.colorBBS(opp));
        if(piece.second){
            result[getIndex(false, piece.first, square)] = 1;
        }
    }

    return result;
}

std::array<int, HIDDEN_LAYER_SIZE> NeuralNetwork::forwardInputLayer(const std::array<int, INPUT_LAYER_SIZE>& inputLayer){
    std::array<int, HIDDEN_LAYER_SIZE> result = {};
    for(int i = 0; i < INPUT_LAYER_SIZE; i++){
        if(!inputLayer[i]) continue;

        for(int x = 0; x < HIDDEN_LAYER_SIZE; x++){
            result[x] += INPUT_LAYER[i][x];
        }
    }

    for(int hiddenIndex = 0; hiddenIndex < HIDDEN_LAYER_SIZE; hiddenIndex++){
        result[hiddenIndex] += INPUT_LAYER_BIASES[hiddenIndex];
    }

    return result;
}


int NeuralNetwork::forwardHiddenLayer(const std::array<int, HIDDEN_LAYER_SIZE * 2>& hiddenLayer){
    int result = 0;

    for(int i = 0; i < HIDDEN_LAYER_SIZE * 2; i++){
        result += crelu(hiddenLayer[i]) * HIDDEN_LAYER_WEIGHTS[i];
    }
    return result;
}

int NeuralNetwork::eval(const Board& board) {
    auto usInputLayer = board.whoPlay ? getInputLayer<Board::WHITE, Board::BLACK>(board) : getInputLayer<Board::BLACK, Board::WHITE>(board);
    auto oppInputLayer = board.whoPlay ? getInputLayer<Board::BLACK, Board::WHITE>(board) : getInputLayer<Board::WHITE, Board::BLACK>(board);

    auto usForward = forwardInputLayer(usInputLayer);
    auto oppForward = forwardInputLayer(oppInputLayer);

    std::array<int, HIDDEN_LAYER_SIZE * 2> mergedForward;
    for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
        mergedForward[i] = usForward[i];
        mergedForward[i + HIDDEN_LAYER_SIZE] = oppForward[i];
    }

    auto result = forwardHiddenLayer(mergedForward);

    result += HIDDEN_LAYER_BIASES[0];
    result *= SCALE;
    result /= QA * QB;
    return result;
}

void NeuralNetwork::load() {
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

int NeuralNetwork::crelu(int value) {
    return std::clamp(value, 0, QA);
}