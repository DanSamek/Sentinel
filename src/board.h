#ifndef SENTINEL_BOARD_H
#define SENTINEL_BOARD_H

#include <string>
#include <vector>
#include <bitboard.h>

class Board {
    // Bitboards
    Bitboard whitePieces[6];
    Bitboard blackPieces[6];

public:
    /***
     * Loads a fen to a board.
     * @param FEN
     */
    void loadFEN(const std::string& FEN);

    /***
     * Initializes all bitboards to an empty state.
     */
    Board();

private:
    void initPieces(Bitboard* pieces, Bitboard::pieceColor color);
};


#endif //SENTINEL_BOARD_H
