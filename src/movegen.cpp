
#include <movegen.h>
#include "bit_ops.h"

void Movegen::initKnightMoves() {

    uint64_t tmp;
    for(int rank = 0; rank < 8; rank++ ){
        for(int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            tmp = 0ULL;
            for(auto item: KNIGHT_OFFSETS){
                int pos = square + item;
                if(pos < 0 || pos > 63) continue;
                bit_ops::setNthBit(tmp, pos);
            }
            KNIGHT_MOVES[square] = tmp & AND_BITBOARDS[square];
        }
    }
}

void Movegen::initKingMoves() {
    uint64_t tmp;
    for(int rank = 0; rank < 8; rank++ ){
        for(int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            tmp = 0ULL;
            for(auto item: KING_OFFSETS){
                int pos = square + item;
                if(pos < 0 || pos > 63) continue;
                bit_ops::setNthBit(tmp, pos);
            }
            KING_MOVES[square] = tmp & AND_BITBOARDS[square];
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

    uint64_t tmp;
    for(int rank = 0; rank < 8; rank++ ){
        for(int file = 0; file < 8; file++){
            tmp = 0ULL;
            auto squares = getSquares(file, rank);
            for(auto square: squares){
                int pos = square.first * 8 + square.second;
                bit_ops::setNthBit(tmp, pos);
            }
            AND_BITBOARDS[rank * 8 + file] = tmp;
        }
    }
}

void Movegen::generatePawnMoves(uint64_t b, const uint64_t &current, const uint64_t &enemy, const uint64_t& all, int enPassantSquare, bool color) {
    static int PAWN_PUSH[] = {8,16};
    static int PAWN_ATTACKS[] = {7,9};
    // vector<moves> pawnMoves;
    int bit = -1;
    while(bit <= 63){
        bit++;
        if(!bit_ops::getNthBit(b, bit)) continue;
        auto rank = bit / 8;
        auto sign = color == Bitboard::BLACK ? +1 : -1;

        uint64_t moves; // will be removed, now only for printing

        const uint64_t* tmpBitboard = &AND_BITBOARDS[bit]; // Rays for valid move.

        for(auto item: PAWN_PUSH){
            auto tmpBit = bit + (sign*item);
            // is oor, or taken or not in range.
            if(tmpBit < 0 || tmpBit > 63 || bit_ops::getNthBit(all, tmpBit) || !bit_ops::getNthBit(*tmpBitboard, tmpBit)){
                break;
            }
            // pawnMoves.push_back(bit, tmpBit, MOVE);
            // pawnMoves.push_back(bit, tmpBit, PROMOTION);  tmpBit/8 == 0 || tmpBit/8 == 7
            bit_ops::setNthBit(moves, tmpBit);
            if(rank != 1 && rank != 6 ) break;
        }

        for(auto item: PAWN_ATTACKS){
            auto tmpBit = bit + (sign*item);
            // is oor, or taken or not in range.
            if(tmpBit < 0 || tmpBit > 63 || bit_ops::getNthBit(current, tmpBit) || !bit_ops::getNthBit(enemy, tmpBit) || !bit_ops::getNthBit(*tmpBitboard, tmpBit)) continue;
            // pawnMoves.push_back(bit, tmpBit, ATTACK);
            // pawnMoves.push_back(bit, tmpBit, PROMOTION);  tmpBit/8 == 0 || tmpBit/8 == 7
            bit_ops::setNthBit(moves, tmpBit);
        }

        if(enPassantSquare == -1) continue;
        // EN-passant. TODO! WHEN Board.h implemented.
        // using tmpBitboard
    }
}

uint64_t Movegen::generateBishopPins(const uint64_t&enemyBishops, const uint64_t & enemy, const uint64_t  &all, const uint64_t & currentKing) {
    int index = 0;
    while(index <= 63){
        if(!bit_ops::getNthBit(enemyBishops, index)) continue;
        auto rays = Magics::getXRay(all, index, false);
    }
}

uint64_t Movegen::generateRookPins(const uint64_t  &enemyRooks, const uint64_t & enemy, const uint64_t  &all, const uint64_t & currentKing) {
    int index = 0;
    while(index <= 63){
        if(!bit_ops::getNthBit(enemyRooks, index)) continue;
        auto rays = Magics::getXRay(all, index, true);
        if(!(rays & currentKing)) continue; // not valid pins.

        // we have pins.
        auto tmp = (Bitboard(1 << index) ^ currentKing) & all;

        tmp.printBoard();
    }
}
