#include "board.h"

Board::Board() {
    initPieces(whitePieces, Bitboard::WHITE);
    initPieces(blackPieces, Bitboard::BLACK);
}

void Board::initPieces(Bitboard *pieces, Bitboard::pieceColor color) {
    pieces[0] = Bitboard(0, Bitboard::PAWN, color);
    pieces[1] = Bitboard(0, Bitboard::BISHOP, color);
    pieces[2] = Bitboard(0, Bitboard::KNIGHT, color);
    pieces[3] = Bitboard(0, Bitboard::ROOK, color);
    pieces[4] = Bitboard(0, Bitboard::QUEEN, color);
    pieces[5] = Bitboard(0, Bitboard::KING, color);
}