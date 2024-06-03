
#include "movegen.h"

void Movegen::initKnightMoves() {
    Bitboard tmp;
    for(int rank = 0; rank < 8; rank++ ){
        for(int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            tmp.value = 0ULL;
            for(auto item: KNIGHT_OFFSETS){
                int pos = square + item;
                if(pos < 0 || pos > 63) continue;
                tmp.setNthBit(pos);
            }
            KNIGHT_MOVES[square] = tmp.value & AND_BITBOARDS[square];
        }
    }
}

void Movegen::initKingMoves() {
    Bitboard tmp;
    for(int rank = 0; rank < 8; rank++ ){
        for(int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            tmp.value = 0ULL;
            for(auto item: KING_OFFSETS){
                int pos = square + item;
                if(pos < 0 || pos > 63) continue;
                tmp.setNthBit(pos);
            }
            KING_MOVES[square] = tmp.value & AND_BITBOARDS[square];
        }
    }
}

void Movegen::initAndBitsForKKP(){
    auto getSquares = [&](int rank, int file)->std::vector<std::pair<int, int>>{
        std::vector<std::pair<int, int>> result;
        int start_i = std::max(0, file - 2);
        int end_i = std::min(7, file + 2);
        int start_j = std::max(0, rank - 2);
        int end_j = std::min(7, rank + 2);

        for (int i = start_i; i <= end_i; ++i) {
            for (int j = start_j; j <= end_j; ++j) {
                if(i < 0 || j < 0 || j>7 || i > 7) continue;
                result.push_back({i, j});
            }
        }
        return result;
    };

    Bitboard tmp;

    for(int rank = 0; rank < 8; rank++ ){
        for(int file = 0; file < 8; file++){
            tmp.value = 0ULL;
            auto squares = getSquares(file, rank);
            for(auto square: squares){
                int pos = square.first * 8 + square.second;
                tmp.setNthBit(pos);
            }
            AND_BITBOARDS[rank * 8 + file] = tmp.value;
        }
    }
}