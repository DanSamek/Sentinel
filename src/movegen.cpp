#include "movegen.h"
#include "bit_ops.h"

void Movegen::init() {
    if(_initDone) return;
    Magics::init();
    initAndBitsForKKP();
    initKnightMoves();
    initKingMoves();
    initPawnAttacks();
    initPawnPushes();
    _initDone = true;
}

Movegen::Movegen(Board &board, Move *moves): board(board), movesPtr(moves){
    friendlyBits = board.whoPlay ? board.whitePieces : board.blackPieces;
    enemyBits = board.whoPlay ? board.blackPieces : board.whitePieces;
    friendlyMerged = friendlyBits[0] | friendlyBits[1] | friendlyBits[2] | friendlyBits[3] | friendlyBits[4] | friendlyBits[5];
    enemyMerged = enemyBits[0] | enemyBits[1] | enemyBits[2] | enemyBits[3] | enemyBits[4] | enemyBits[5];
    all = friendlyMerged | enemyMerged;
}

void Movegen::generatePromotions(int fromSq, int toSq, bool capture){
    auto flag = capture ? Move::PROMOTION_CAPTURE : Move::PROMOTION;
    movesPtr[index++] = {fromSq, toSq, Move::QUEEN, flag, PIECE_TYPE::PAWN};
    movesPtr[index++] = {fromSq, toSq, Move::BISHOP, flag, PIECE_TYPE::PAWN};
    movesPtr[index++] = {fromSq, toSq, Move::ROOK, flag, PIECE_TYPE::PAWN};
    movesPtr[index++] = {fromSq, toSq, Move::KNIGHT, flag, PIECE_TYPE::PAWN};
}

// will be used for all except pawns || kings?
void Movegen::bitboardToMoves(int fromSquare, uint64_t& moveBitboard, PIECE_TYPE pieceType) {
    while(moveBitboard){
        int toSquare = bit_ops::bitScanForwardPopLsb(moveBitboard);
        if (bit_ops::getNthBit(friendlyMerged, toSquare)) continue; // cant go on friendly piece.
        Move::type mt = bit_ops::getNthBit(enemyMerged, toSquare) ? Move::CAPTURE : Move::QUIET;
        movesPtr[index++] = {fromSquare, toSquare, Move::NONE, mt, pieceType};
    }
}

void Movegen::generateRookMoves(uint64_t rooks) {
    while(rooks){
        int pos = bit_ops::bitScanForwardPopLsb(rooks);
        auto movesBitboard = Magics::getRookMoves(all, pos);
        bitboardToMoves(pos, movesBitboard, PIECE_TYPE::ROOK);
    }
}

void Movegen::generateBishopMoves(uint64_t bishops) {
    while(bishops){
        int pos = bit_ops::bitScanForwardPopLsb(bishops);
        auto movesBitboard = Magics::getBishopMoves(all, pos);
        bitboardToMoves(pos, movesBitboard, PIECE_TYPE::BISHOP);
    }
}

void Movegen::generateQueenMoves(uint64_t queens) {
    while (queens){
        int pos = bit_ops::bitScanForwardPopLsb(queens);
        auto movesBitboard = Magics::getBishopMoves(all, pos);
        movesBitboard |= Magics::getRookMoves(all, pos);
        bitboardToMoves(pos, movesBitboard, PIECE_TYPE::QUEEN);
    }
}

void Movegen::generateKnightMoves(uint64_t knight) {
    while(knight){
        int pos = bit_ops::bitScanForwardPopLsb(knight);
        auto bb = KNIGHT_MOVES[pos];
        bitboardToMoves(pos, bb, PIECE_TYPE::KNIGHT);
    }
}

void Movegen::generateKingMoves(uint64_t king, const std::array<bool,2>& castling) {
    auto pos = bit_ops::bitScanForwardPopLsb(king);
    auto bb = KING_MOVES[pos];
    bitboardToMoves(pos, bb, PIECE_TYPE::KING);
    // Note, in board.cpp we handle castling rights !!!
    auto castlingMasks = CASTLING_FREE_MASKS[!board.whoPlay];
    if(castling[Board::K_CASTLE] && (castlingMasks[Board::K_CASTLE] & all) == 0){
        movesPtr[index++] = {pos, pos + 2, Move::NONE, Move::CASTLING, PIECE_TYPE::KING};
    }
    if(castling[Board::Q_CASTLE] && (castlingMasks[Board::Q_CASTLE] & all) == 0){
        movesPtr[index++] = {pos, pos - 2, Move::NONE, Move::CASTLING, PIECE_TYPE::KING};
    }
}

bool Movegen::validateKingCheck(int kingPos) {
    // generate all possible moves FROM a king perspective.
    if(Magics::getRookMoves(all, kingPos) & (enemyBits[PIECE_TYPE::ROOK] | enemyBits[PIECE_TYPE::QUEEN])) return false;

    if(Magics::getBishopMoves(all, kingPos) & (enemyBits[PIECE_TYPE::BISHOP] | enemyBits[PIECE_TYPE::QUEEN])) return false;

    if(KNIGHT_MOVES[kingPos] & enemyBits[PIECE_TYPE::KNIGHT]) return false;

    if(PAWN_ATTACK_MOVES[board.whoPlay][kingPos] & enemyBits[PIECE_TYPE::PAWN]) return false;

    if(KING_MOVES[kingPos] & enemyBits[PIECE_TYPE::KING]) return false;
    return true;
}


/* All captures */
void Movegen::generateRookCaptures(uint64_t rooks) {
    while(rooks){
        int pos = bit_ops::bitScanForwardPopLsb(rooks);
        auto movesBitboard = Magics::getRookMoves(all, pos) & enemyMerged;
        captureBitboardToMoves(pos, movesBitboard, PIECE_TYPE::ROOK);
    }
}


void Movegen::generateBishopCaptures(uint64_t bishops) {
    while(bishops){
        int pos = bit_ops::bitScanForwardPopLsb(bishops);
        auto movesBitboard = Magics::getBishopMoves(all, pos) & enemyMerged;
        captureBitboardToMoves(pos, movesBitboard, PIECE_TYPE::BISHOP);
    }
}

void Movegen::generateQueenCaptures(uint64_t queens) {
    while (queens){
        int pos = bit_ops::bitScanForwardPopLsb(queens);
        auto movesBitboard = Magics::getBishopMoves(all, pos);
        movesBitboard |= Magics::getRookMoves(all, pos);
        movesBitboard &= enemyMerged;
        captureBitboardToMoves(pos, movesBitboard, PIECE_TYPE::QUEEN);
    }
}

void Movegen::generateKnightCaptures(uint64_t knight) {
    while(knight){
        int pos = bit_ops::bitScanForwardPopLsb(knight);
        auto bb = KNIGHT_MOVES[pos] & enemyMerged;
        bitboardToMoves(pos, bb, PIECE_TYPE::KNIGHT);
    }
}

void Movegen::generateKingCaptures(uint64_t king) {
    auto pos = bit_ops::bitScanForwardPopLsb(king);
    auto bb = KING_MOVES[pos] & enemyMerged;
    captureBitboardToMoves(pos, bb, PIECE_TYPE::KING);
}

void Movegen::captureBitboardToMoves(int fromSquare, uint64_t& moveBitboard, PIECE_TYPE pieceType){
    while(moveBitboard){
        int toSquare = bit_ops::bitScanForwardPopLsb(moveBitboard);
        movesPtr[index++] = {fromSquare, toSquare, Move::NONE, Move::CAPTURE, pieceType};
    }
}


/*  */
/* LOOKUP TABLES IMPLEMENTATIONS  */
/* SORRY, GARBAGE CODE, I DONT CARE :3 */
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
            PAWN_ATTACK_MOVES[PIECE_COLOR::WHITE][square] = tmp & AND_BITBOARDS[square];

            // white black color
            sign = +1;
            tmp = 0ULL;
            for(auto item: PAWN_ATTACKS){
                auto tmpBit = square + (sign*item);
                if(tmpBit < 0 || tmpBit > 63) continue;
                bit_ops::setNthBit(tmp, tmpBit);
            }
            PAWN_ATTACK_MOVES[PIECE_COLOR::BLACK][square] = tmp & AND_BITBOARDS[square];
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

            PAWN_PUSH_MOVES[PIECE_COLOR::WHITE][square] = tmp;
            tmp = PAWN_PUSH_MOVES[PIECE_COLOR::WHITE][square];

            if(first >= 0 && first <= 63) bit_ops::popNthBit(tmp, first);
            PAWN_ILLEGAL_AND[PIECE_COLOR::WHITE][square] = rank != 6 ? 1ULL : tmp;


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
            PAWN_PUSH_MOVES[PIECE_COLOR::BLACK][square] = tmp;
            tmp = PAWN_PUSH_MOVES[PIECE_COLOR::BLACK][square];
            if(first >= 0 && first <= 63) bit_ops::popNthBit(tmp, first);
            PAWN_ILLEGAL_AND[PIECE_COLOR::BLACK][square] = rank != 1 ? 1ULL : tmp;
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

