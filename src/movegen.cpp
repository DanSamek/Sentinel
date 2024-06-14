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

void Movegen::initPawnPushes(){
    uint64_t tmp;
    for(int rank = 0; rank < 8; rank++ ){
        for(int file = 0; file < 8; file++){
            // white color
            int square = rank * 8 + file;
            tmp = 0ULL;
            auto sign = -1;
            int first = -1;
            for(auto item: PAWN_PUSH){
                auto tmpBit = square + (sign*item);
                if(tmpBit < 0 || tmpBit > 63) continue;
                first = tmpBit;
                bit_ops::setNthBit(tmp, tmpBit);
                if(rank != 6) break;
            }

            PAWN_PUSH_MOVES[Board::WHITE][square] = tmp;
            tmp = PAWN_PUSH_MOVES[Board::WHITE][square];

            bit_ops::popNthBit(tmp, first);
            PAWN_ILLEGAL_AND[Board::WHITE][square] = rank != 6 ? 1ULL : tmp;


            // white black color
            sign = +1;
            tmp = 0ULL;
            first = -1;
            for(auto item: PAWN_PUSH){
                auto tmpBit = square + (sign*item);
                if(tmpBit < 0 || tmpBit > 63) continue;
                first = tmpBit;
                bit_ops::setNthBit(tmp, tmpBit);
                if(rank != 1) break;
            }
            PAWN_PUSH_MOVES[Board::BLACK][square] = tmp;
            tmp = PAWN_PUSH_MOVES[Board::BLACK][square];
            bit_ops::popNthBit(tmp, first);
            PAWN_ILLEGAL_AND[Board::BLACK][square] = rank != 1 ? 1ULL : tmp;
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


static void generatePromotions(std::vector<Move>& moves, int fromSq, int toSq){
    moves[Movegen::index++] = {fromSq, toSq, Move::QUEEN, Move::PROMOTION, Board::PAWN};
    moves[Movegen::index++] = {fromSq, toSq, Move::BISHOP, Move::PROMOTION, Board::PAWN};
    moves[Movegen::index++] = {fromSq, toSq, Move::ROOK, Move::PROMOTION, Board::PAWN};
    moves[Movegen::index++] = {fromSq, toSq, Move::KNIGHT, Move::PROMOTION, Board::PAWN};
}

void Movegen::generatePawnMoves(uint64_t b, const uint64_t &current, const uint64_t &enemy, const uint64_t& all, int enPassantSquare, bool color, std::vector<Move>& moves) {
    while(b){
        int bit = bit_ops::bitScanForwardPopLsb(b);
        int rank = bit / 8;

        auto bb = PAWN_PUSH_MOVES[!color][bit];
        if(((color == Bitboard::WHITE && rank == 6) || (color == Bitboard::BLACK && rank == 1)) && (bb & all) != PAWN_ILLEGAL_AND[!color][bit]){
            bb &= ~all;
            while(bb){
                auto tmpBit = bit_ops::bitScanForwardPopLsb(bb);
                auto tmpRank = tmpBit/8;
                if((color == Bitboard::WHITE && tmpRank == 0) || (color == Bitboard::BLACK && tmpRank == 7)){
                    generatePromotions(moves, bit, tmpBit);
                }
                else{
                    moves[Movegen::index++] = {bit, tmpBit, Move::NONE, Move::QUIET, Board::PAWN};
                }
            }
        }
        else{
            while(!(bb & all) && bb){
                auto tmpBit = bit_ops::bitScanForwardPopLsb(bb);
                auto tmpRank = tmpBit/8;
                if((color == Bitboard::WHITE && tmpRank == 0) || (color == Bitboard::BLACK && tmpRank == 7)){
                    generatePromotions(moves, bit, tmpBit);
                }
                else{
                    moves[Movegen::index++] = {bit, tmpBit, Move::NONE, Move::QUIET, Board::PAWN};
                }
            }
        }

        bb = PAWN_ATTACK_MOVES[!color][bit];
        // normal captures || captures with promotions.
        while(bb){
            auto tmpBit = bit_ops::bitScanForwardPopLsb(bb);
            auto capture = bit_ops::getNthBit(enemy, tmpBit);
            if(((color == Bitboard::WHITE && tmpBit/8 == 0) || (color == Bitboard::BLACK && tmpBit/8 == 7)) && capture){
                generatePromotions(moves, bit, tmpBit);
            }
            else if(tmpBit == enPassantSquare && !bit_ops::getNthBit(all, tmpBit)){
                moves[Movegen::index++] = {bit, tmpBit, Move::NONE, Move::EN_PASSANT, Board::PAWN};
            }
            else if(capture){
                moves[Movegen::index++] = {bit, tmpBit, Move::NONE, Move::CAPTURE, Board::PAWN};
            }
        }
    }
}


uint64_t Movegen::generateConstantEnemyAttacks(const uint64_t& king, const std::vector<uint64_t>& enemyPieces, bool enemyColor) {
    int checkCount = 0;
    uint64_t attacks = 0ULL;
    uint64_t current = attacks;
    // enemy king attack gen
    auto enemyKing = enemyPieces[Board::KING];
    while(enemyKing){
        int pos = bit_ops::bitScanForwardPopLsb(enemyKing);
        attacks |= KING_MOVES[pos];
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

    return attacks;
}

void Movegen::incrementIfKingChecked(const uint64_t &king, const uint64_t &attacks, int& checkCount) {
    checkCount += (king & attacks) != 0ULL;
}

// will be used for all except pawns || kings?
void Movegen::bitboardToMoves(int fromSquare, uint64_t moveBitboard, std::vector<Move> &moves, const uint64_t& enemies, const uint64_t& enemyKing, Board::pieceType pieceType, const uint64_t& friendly) {
    while(moveBitboard){
        int toSquare = bit_ops::bitScanForwardPopLsb(moveBitboard);
        auto bitboard = 1ULL << toSquare;
        if((bitboard & friendly)) continue; // cant go on friendly piece.
        Move::type mt = bitboard & enemyKing ? Move::CHECK : enemies & bitboard ? Move::CAPTURE : Move::QUIET;
        moves[Movegen::index++] = {fromSquare, toSquare, Move::NONE, mt, pieceType};
    }
}

void Movegen::generateRookMoves(uint64_t rooks, const uint64_t& friendlyMerged, const uint64_t& enemyMerged, const uint64_t& all, std::vector<Move> &moves, const uint64_t& enemyKing) {
    while(rooks){
        int pos = bit_ops::bitScanForwardPopLsb(rooks);
        auto movesBitboard = Magics::getRookMoves(all, pos);
        bitboardToMoves(pos, movesBitboard, moves, enemyMerged, enemyKing, Board::ROOK, friendlyMerged);
    }
}

void Movegen::generateBishopMoves(uint64_t bishops, const uint64_t& friendlyMerged, const uint64_t& enemyMerged, const uint64_t& all, std::vector<Move> &moves, const uint64_t& enemyKing) {
    while(bishops){
        int pos = bit_ops::bitScanForwardPopLsb(bishops);
        auto movesBitboard = Magics::getBishopMoves(all, pos);
        bitboardToMoves(pos, movesBitboard, moves, enemyMerged, enemyKing, Board::BISHOP, friendlyMerged);
    }
}

void Movegen::generateQueenMoves(uint64_t queens, const uint64_t& friendlyMerged, const uint64_t& enemyMerged, const uint64_t& all, std::vector<Move> &moves, const uint64_t& enemyKing) {
    while (queens){
        int pos = bit_ops::bitScanForwardPopLsb(queens);
        auto movesBitboard = Magics::getBishopMoves(all, pos);
        movesBitboard |= Magics::getRookMoves(all, pos);
        bitboardToMoves(pos, movesBitboard, moves, enemyMerged, enemyKing, Board::QUEEN, friendlyMerged);
    }
}

void Movegen::generateKnightMoves(uint64_t knight, const uint64_t& friendlyMerged, const uint64_t& enemyMerged, const uint64_t& all, std::vector<Move> &moves, const uint64_t& enemyKing) {
    while(knight){
        int pos = bit_ops::bitScanForwardPopLsb(knight);
        auto bb = KNIGHT_MOVES[pos];
        bitboardToMoves(pos, bb, moves, enemyMerged, enemyKing, Board::KNIGHT, friendlyMerged);
    }
}

void Movegen::generateKingMoves(uint64_t king, const uint64_t& friendlyMerged, const uint64_t& enemyMerged, const uint64_t& all, std::vector<Move> &moves, const bool castling[2], bool whoPlay) {
    auto pos = bit_ops::bitScanForwardPopLsb(king);
    auto bb = KING_MOVES[pos];
    bitboardToMoves(pos, bb, moves, enemyMerged, 0, Board::KING, friendlyMerged);
    // board.makeMove will handle castling (if its possible)
    // we can check, if something is between rooks and kings <-> not possible.
    auto castlingMasks = CASTLING_FREE_MASKS[!whoPlay];
    if(castling[Board::K_CASTLE] && (castlingMasks[0] & all) == 0){
        moves[Movegen::index++] = {pos, pos + 2, Move::NONE, Move::CASTLING, Board::KING};
    }
    if(castling[Board::Q_CASTLE] && (castlingMasks[1] & all) == 0){
        moves[Movegen::index++] = {pos, pos - 3, Move::NONE, Move::CASTLING, Board::KING};
    }
}
