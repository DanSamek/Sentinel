#include <movegen.h>
#include "bit_ops.h"

/*  */
/* LOOKUP TABLES IMPLEMENTATIONS  */
/*  */

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

void Movegen::initPawnAttacks(){
    uint64_t tmp;
    for(int rank = 0; rank < 8; rank++ ){
        for(int file = 0; file < 8; file++){
            // white color
            int square = rank * 8 + file;
            tmp = 0ULL;
            auto sign = -1;
            for(auto item: PAWN_ATTACKS){
                auto tmpBit = square + (sign*item);
                if(tmpBit < 0 || tmpBit > 63) continue;
                bit_ops::setNthBit(tmp, tmpBit);
            }
            PAWN_ATTACK_MOVES[Board::WHITE][square] = tmp & AND_BITBOARDS[square];

            // white black color
            sign = +1;
            tmp = 0ULL;
            for(auto item: PAWN_ATTACKS){
                auto tmpBit = square + (sign*item);
                if(tmpBit < 0 || tmpBit > 63) continue;
                bit_ops::setNthBit(tmp, tmpBit);
            }
            PAWN_ATTACK_MOVES[Board::BLACK][square] = tmp & AND_BITBOARDS[square];
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

/*  */
/* LOOKUP TABLES IMPLEMENTATIONS  END */
/*  */


void Movegen::generatePawnMoves(uint64_t b, const uint64_t &current, const uint64_t &enemy, const uint64_t& all, int enPassantSquare, bool color) {
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
            // pawnMoves.push_back(bit, tmpBit, {PROMOTION} -> {QUEEN, ROOK, BISHOP, KNIGHT});  tmpBit/8 == 0 || tmpBit/8 == 7
            bit_ops::setNthBit(moves, tmpBit);
            if(rank != 1 && rank != 6 ) break;
        }

        for(auto item: PAWN_ATTACKS){
            auto tmpBit = bit + (sign*item);
            // is oor, or taken or not in range.
            if(tmpBit < 0 || tmpBit > 63 || !bit_ops::getNthBit(*tmpBitboard, tmpBit)) continue;

            // !bit_ops::getNthBit(enemy, tmpBit) || bit_ops::getNthBit(current, tmpBit)
            if(tmpBit == enPassantSquare && !bit_ops::getNthBit(all, tmpBit)){
                // en passant
                // pawnMoves.push_back(bit, tmpBit, EN_PASSANT);
                continue;
            }

            // is there enemy -> capture
            if(bit_ops::getNthBit(enemy, tmpBit)){
                // pawnMoves.push_back(bit, tmpBit, ATTACK);
                // what if its end rank?
                // pawnMoves.push_back(bit, tmpBit, PROMOTION -> {QUEEN, ROOK, BISHOP, KNIGHT});  tmpBit/8 == 0 || tmpBit/8 == 7
            }
            bit_ops::setNthBit(moves, tmpBit);
        }
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


// Bit messy, but it will generate all attacks with checks.
std::pair<int, uint64_t> Movegen::getKingChecksAndAttacks(const uint64_t& king, const std::vector<uint64_t>& enemyPieces, const uint64_t& all, bool enemyColor) {
    int checkCount = 0;
    uint64_t attacks = 0ULL;
    uint64_t current = attacks;
    // enemy king attack gen
    auto enemyKing = enemyPieces[Board::KING];
    while(enemyKing){
        int pos = bit_ops::bitScanForwardPopLsb(enemyKing);
        attacks |= KING_MOVES[pos];
    }
    // enemy rooks attack gen
    auto rooks = enemyPieces[Board::ROOK];
    while(rooks){
        int pos = bit_ops::bitScanForwardPopLsb(rooks);
        current = Magics::getSlidingMoves(all, pos, true);
        incrementIfKingChecked(king, current, checkCount);
        attacks |= current;
    }

    // enemy bishops attack gen.
    auto bishops = enemyPieces[Board::BISHOP];
    while(bishops){
        int pos = bit_ops::bitScanForwardPopLsb(bishops);
        current = Magics::getSlidingMoves(all, pos, false);
        incrementIfKingChecked(king, current, checkCount);
        attacks |= current;
    }

    // enemy queen attack gen.
    auto queens = enemyPieces[Board::QUEEN];
    while(queens){
        int pos = bit_ops::bitScanForwardPopLsb(queens);
        current = Magics::getSlidingMoves(all, pos, false);
        incrementIfKingChecked(king, current, checkCount);
        attacks |= current;

        current = Magics::getSlidingMoves(all, pos, true);
        incrementIfKingChecked(king, current, checkCount);
        attacks |= current;
    }

    auto knights = enemyPieces[Board::KNIGHT];
    while(knights){
        int pos = bit_ops::bitScanForwardPopLsb(knights);
        current = KNIGHT_MOVES[pos];
        incrementIfKingChecked(king, current, checkCount);
        attacks |= current;
    }

    auto pawns = enemyPieces[Board::PAWN];
    auto color = enemyColor ? 0 : 1;

    while(pawns){
        int pos = bit_ops::bitScanForwardPopLsb(pawns);
        current = PAWN_ATTACK_MOVES[color][pos];
        incrementIfKingChecked(king, current, checkCount);
        attacks |= current;
    }

    return {checkCount,attacks};
}

void Movegen::incrementIfKingChecked(const uint64_t &king, const uint64_t &attacks, int& checkCount) {
    checkCount += (king & attacks) != 0ULL;
}

// will be used for all except pawns || kings?
void Movegen::bitboardToMoves(int fromSquare, uint64_t moveBitboard, std::vector<Move> &moves, const uint64_t& enemies, const uint64_t& enemyKing, Board::pieceType pieceType) {
    while(moveBitboard){
        int toSquare = bit_ops::bitScanForward(moveBitboard);
        auto bitboard = 1ULL << toSquare;
        Move::type mt = bitboard & enemyKing ? Move::CHECK : enemies & bitboard ? Move::CAPTURE : Move::QUIET;
        moves.push_back({fromSquare, toSquare, Move::NONE, mt, pieceType});
        moveBitboard &= moveBitboard - 1;
    }
}
