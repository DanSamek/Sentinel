#include "sstream"
#include "nnue.h"
#include "fstream"
#include "cassert"
#include "singularity_v2_4_40.bin.hpp"
#include "development.h"

namespace Sentinel{

    template<typename T>
    T readNumber(std::ifstream & stream){
        T value;
        stream.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }

    template<typename T>
    T readNumber(std::istringstream & stream){
        T value;
        stream.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }

    NNUE::NNUE() {
        load();
        reset();
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
    #if DEVELOPMENT
        std::cout << "loading net, inline: " << inlineNet << std::endl;
    #endif

        if(!inlineNet) loadFromFile();
        else loadInlined();
    }

    void NNUE::moveAPiece(PIECE_COLOR color, PIECE_TYPE piece, int fromSquare, int toSquare) {
        updateAccumulatorSub(color, piece, fromSquare); // -> 0
        updateAccumulatorAdd(color, piece, toSquare); // -> 1
    }

    #if DEVELOPMENT
    void NNUE::print(){
        return;
        for(int i = 0; i < INPUT_LAYER_SIZE; i++){
            for(int x = 0; x < HIDDEN_LAYER_SIZE; x++){
                std::cout << INPUT_LAYER[i][x] << " ";
            }
        }
        for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
            std::cout << INPUT_LAYER_BIASES[i] << " ";
        }

        for(int i = 0; i < HIDDEN_LAYER_SIZE * 2; i++){
            std::cout << HIDDEN_LAYER_WEIGHTS[i] << " ";
        }

        std::cout <<HIDDEN_LAYER_BIASES[0] << std::endl;
    }
    #endif

    /*
     * !!!!
     * !!!!
     * TODO
     * !!!!
     * !!!!
     */

    void NNUE::loadFromFile() {
        auto stream = std::ifstream(NET_PATH, std::ios::binary);
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

    #if DEVELOPMENT
        print();
    #endif

    }

    void NNUE::loadInlined() {
        std::istringstream stream;
        auto size = sizeof(singularity_v2_4_40_bin) / sizeof (unsigned char);
        stream.rdbuf()->pubsetbuf((char *) singularity_v2_4_40_bin, size);

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

    #if DEVELOPMENT
        print();
    #endif
    }

}