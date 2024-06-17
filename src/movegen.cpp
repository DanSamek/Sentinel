#include <movegen.h>
#include <bit_ops.h>

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

            if(first >= 0 && first <= 63) bit_ops::popNthBit(tmp, first);
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
            if(first >= 0 && first <= 63) bit_ops::popNthBit(tmp, first);
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
/* LOOKUP TABLES IMPLEMENTATIONS  END */


void Movegen::generatePawnMoves(uint64_t b, int enPassantSquare, bool color) {
    uint64_t enPassantBB = enPassantSquare != -1 ? 1ULL << enPassantSquare : 0;
    while(b){
        int bit = bit_ops::bitScanForwardPopLsb(b);
        int rank = bit / 8;
        bool promotion = (color == Bitboard::WHITE && rank == 1) ||(color == Bitboard::BLACK && rank == 6); // next move will be promotion on 100%!

        auto bb = PAWN_PUSH_MOVES[!color][bit];
        if(((color == Bitboard::WHITE && rank == 6) || (color == Bitboard::BLACK && rank == 1)) && (bb & all) != PAWN_ILLEGAL_AND[!color][bit]){
            bb &= ~all;
            while(bb){
                auto tmpBit = bit_ops::bitScanForwardPopLsb(bb);
                tmpMoves[Movegen::index++] = {bit, tmpBit, Move::NONE, abs(tmpBit - bit) > 8 ? Move::DOUBLE_PAWN_UP : Move::QUIET, Board::PAWN};
            }
        }
        else{
            while(!(bb & all) && bb){
                auto tmpBit = bit_ops::bitScanForwardPopLsb(bb);
                if(promotion) generatePromotions(bit, tmpBit);
                else tmpMoves[Movegen::index++] = {bit, tmpBit, Move::NONE, Move::QUIET, Board::PAWN};
            }
        }

        bb = PAWN_ATTACK_MOVES[!color][bit] & (enemyMerged | enPassantBB);
        // normal captures || captures with promotions.
        while(bb){
            auto tmpBit = bit_ops::bitScanForwardPopLsb(bb);
            if(tmpBit == enPassantSquare) tmpMoves[Movegen::index++] = {bit, tmpBit, Move::NONE, Move::EN_PASSANT, Board::PAWN};
            else if(promotion) generatePromotions(bit, tmpBit);
            else tmpMoves[Movegen::index++] = {bit, tmpBit, Move::NONE, Move::CAPTURE, Board::PAWN};
        }
    }
}

void Movegen::generatePromotions(int fromSq, int toSq){
    tmpMoves[Movegen::index++] = {fromSq, toSq, Move::QUEEN, Move::PROMOTION, Board::PAWN};
    tmpMoves[Movegen::index++] = {fromSq, toSq, Move::BISHOP, Move::PROMOTION, Board::PAWN};
    tmpMoves[Movegen::index++] = {fromSq, toSq, Move::ROOK, Move::PROMOTION, Board::PAWN};
    tmpMoves[Movegen::index++] = {fromSq, toSq, Move::KNIGHT, Move::PROMOTION, Board::PAWN};
}

void Movegen::incrementIfKingChecked(const uint64_t &king, const uint64_t &attacks, int& checkCount) {
    checkCount += (king & attacks) != 0ULL;
}

// will be used for all except pawns || kings?
void Movegen::bitboardToMoves(int fromSquare, uint64_t moveBitboard, Board::pieceType pieceType) {
    while(moveBitboard){
        int toSquare = bit_ops::bitScanForwardPopLsb(moveBitboard);
        if (bit_ops::getNthBit(friendlyMerged, toSquare)) continue; // cant go on friendly piece.
        Move::type mt = bit_ops::getNthBit(enemyMerged, toSquare) ? Move::CAPTURE : Move::QUIET;
        tmpMoves[Movegen::index++] = {fromSquare, toSquare, Move::NONE, mt, pieceType};
    }
}

void Movegen::generateRookMoves(uint64_t rooks) {
    while(rooks){
        int pos = bit_ops::bitScanForwardPopLsb(rooks);
        auto movesBitboard = Magics::getRookMoves(all, pos);
        bitboardToMoves(pos, movesBitboard, Board::ROOK);
    }
}

void Movegen::generateBishopMoves(uint64_t bishops) {
    while(bishops){
        int pos = bit_ops::bitScanForwardPopLsb(bishops);
        auto movesBitboard = Magics::getBishopMoves(all, pos);
        bitboardToMoves(pos, movesBitboard, Board::BISHOP);
    }
}

void Movegen::generateQueenMoves(uint64_t queens) {
    while (queens){
        int pos = bit_ops::bitScanForwardPopLsb(queens);
        auto movesBitboard = Magics::getBishopMoves(all, pos);
        movesBitboard |= Magics::getRookMoves(all, pos);
        bitboardToMoves(pos, movesBitboard, Board::QUEEN);
    }
}

void Movegen::generateKnightMoves(uint64_t knight) {
    while(knight){
        int pos = bit_ops::bitScanForwardPopLsb(knight);
        auto bb = KNIGHT_MOVES[pos];
        bitboardToMoves(pos, bb, Board::KNIGHT);
    }
}

void Movegen::generateKingMoves(uint64_t king, const std::array<bool,2>& castling, bool whoPlay) {
    auto pos = bit_ops::bitScanForwardPopLsb(king);
    auto bb = KING_MOVES[pos];
    bitboardToMoves(pos, bb, Board::KING);
    // Note, in board.cpp we handle castling rights !!!
    auto castlingMasks = CASTLING_FREE_MASKS[!whoPlay];
    if(castling[Board::K_CASTLE] && (castlingMasks[0] & all) == 0){
        tmpMoves[Movegen::index++] = {pos, pos + 2, Move::NONE, Move::CASTLING, Board::KING};
    }
    if(castling[Board::Q_CASTLE] && (castlingMasks[1] & all) == 0){
        tmpMoves[Movegen::index++] = {pos, pos - 3, Move::NONE, Move::CASTLING, Board::KING};
    }
}

bool Movegen::validateKingCheck(int kingPos, bool whoPlay, uint64_t *enemyBits) {
    // generate all possible moves FROM a king perspective.
    // sliders
    // rook || queen
    if(Magics::getRookMoves(all, kingPos) & (enemyBits[Board::ROOK] | enemyBits[Board::QUEEN])) return false;

    if(Magics::getBishopMoves(all, kingPos) & (enemyBits[Board::BISHOP] | enemyBits[Board::QUEEN])) return false;

    // knights.
    if(KNIGHT_MOVES[kingPos] & enemyBits[Board::KNIGHT]) return false;

    // pawns
    if(PAWN_ATTACK_MOVES[whoPlay][kingPos] & enemyBits[Board::PAWN]) return false;

    // kings.
    if(KING_MOVES[kingPos] & enemyBits[Board::KING]) return false;
    return true;
}
