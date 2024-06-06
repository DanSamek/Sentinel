#ifndef SENTINEL_BOARD_H
#define SENTINEL_BOARD_H

#include <map>
#include <string>
#include <vector>
#include <bitboard.h>

class Board {
    static inline std::map<char, int> pieceIndexMap = {{'p',0}, {'n', 1}, {'b', 2}, {'r',3}, {'q',4}, {'k', 5}};
    // lazy mapping.
    static inline std::map<char, int> files = {{'a', 0}, {'b', 1}, {'c', 2}, {'d',3}, {'e', 4}, {'f', 5}, {'g', 6}, {'h',7}};
    static inline std::map<char, int> ranks = {{'1',7}, {'2',6},{'3',5},{'4',4}, {'5',3}, {'6',2}, {'7',1}, {'8',0}};
public:
    // Bitboards
    std::vector<Bitboard> whitePieces; // make it easy
    std::vector<Bitboard> blackPieces;

    // enPassantSquare is set to -1 if there is no enpassant
    int halfMove, fullMove, enPassantSquare;
    // true -> white, false -> black
    bool whoPlay;

    // WHITE, BLACK {queen, king}
    static inline int K_CASTLE = 1;
    static inline int Q_CASTLE = 0;
    bool castling [2][2];

    /***
     * Loads a fen to a board.
     * @param FEN
     */
    void loadFEN(const std::string FEN);

    /***
     * Initializes all bitboards to an empty state.
     */
    Board();

    /***
     * @param type
     * @param color
     * @return bitboard for a request.
     */
    const Bitboard& getPieceBitboard(Bitboard::pieceType type, Bitboard::pieceColor color) const;
private:
    void initPieces(std::vector<Bitboard>& pieces, Bitboard::pieceColor color);
};


#endif //SENTINEL_BOARD_H
