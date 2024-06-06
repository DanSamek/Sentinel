
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
            KNIGHT_MOVES[square] = Bitboard(tmp.value & AND_BITBOARDS[square].value);
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
            KING_MOVES[square] = Bitboard(tmp.value & AND_BITBOARDS[square].value);
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
            AND_BITBOARDS[rank * 8 + file] = Bitboard(tmp.value);
        }
    }
}

void Movegen::generatePawnMoves(Bitboard b, const Bitboard &current, const Bitboard &enemy, const Bitboard& all, int enPassantSquare) {
    static int PAWN_PUSH[] = {8,16};
    static int PAWN_ATTACKS[] = {7,9};
    // vector<moves> pawnMoves;
    int bit = -1;
    while(bit <= 63){
        bit++;
        if(!b.getNthBit(bit)) continue;
        auto rank = bit / 8;
        auto sign = b.color == Bitboard::BLACK ? +1 : -1;

        Bitboard moves; // will be removed, now only for printing

        const Bitboard* tmpBitboard = &AND_BITBOARDS[bit]; // Rays for valid move.

        for(auto item: PAWN_PUSH){
            auto tmpBit = bit + (sign*item);
            // is oor, or taken or not in range.
            if(tmpBit < 0 || tmpBit > 63 || all.getNthBit(tmpBit) || !tmpBitboard->getNthBit(tmpBit)){
                break;
            }
            // pawnMoves.push_back(bit, tmpBit, MOVE);
            // pawnMoves.push_back(bit, tmpBit, PROMOTION);  tmpBit/8 == 0 || tmpBit/8 == 7
            moves.setNthBit(tmpBit);
            if(rank != 1 && rank != 6 ) break;
        }

        for(auto item: PAWN_ATTACKS){
            auto tmpBit = bit + (sign*item);
            // is oor, or taken or not in range.
            if(tmpBit < 0 || tmpBit > 63 || current.getNthBit(tmpBit) || !enemy.getNthBit(tmpBit) || !tmpBitboard->getNthBit(tmpBit)) continue;
            // pawnMoves.push_back(bit, tmpBit, ATTACK);
            // pawnMoves.push_back(bit, tmpBit, PROMOTION);  tmpBit/8 == 0 || tmpBit/8 == 7
            moves.setNthBit(tmpBit);
        }

        if(enPassantSquare == -1) continue;
        // EN-passant. TODO! WHEN Board.h implemented.
        // using tmpBitboard
    }
}

Bitboard Movegen::generateBishopPins(const Bitboard &enemyBishops, const Bitboard& enemy, const Bitboard &all, const Bitboard& currentKing) {
    int index = 0;
    while(index <= 63){
        if(!enemyBishops.getNthBit(index)) continue;
        auto rays = Magics::getXRay(all.value, index, false);
    }
}

Bitboard Movegen::generateRookPins(const Bitboard &enemyRooks, const Bitboard& enemy, const Bitboard &all, const Bitboard& currentKing) {
    int index = 0;
    while(index <= 63){
        if(!enemyRooks.getNthBit(index)) continue;
        auto rays = Magics::getXRay(all.value, index, true);
        if(!(rays & currentKing.value)) continue; // not valid pins.

        // we have pins.
        auto tmp = (Bitboard(1 << index) ^ currentKing) & all;

        tmp.printBoard();
    }
}
