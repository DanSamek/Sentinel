#ifndef SENTINEL_MOVEGEN_H
#define SENTINEL_MOVEGEN_H

#include <vector>
#include "magics.h"
#include "move.h"
class Board;
#include "board.h"
#include <span>

#define UPDATE_BOARD_STATE(board, whoPlay) \
    auto friendlyBits = whoPlay ? board.whitePieces : board.blackPieces; \
    auto enemyBits = whoPlay ? board.blackPieces : board.whitePieces; \
    Movegen::_friendlyMerged = friendlyBits[0] | friendlyBits[1] | friendlyBits[2] | friendlyBits[3] | friendlyBits[4] | friendlyBits[5]; \
    Movegen::_enemyMerged = enemyBits[0] | enemyBits[1] | enemyBits[2] | enemyBits[3] | enemyBits[4] | enemyBits[5]; \
    Movegen::_all = Movegen::_friendlyMerged | Movegen::_enemyMerged

/*
    !!! DOING PSEUDO LEGAL MOVE GENERATION !!!
        AKA generate all moves, check if moves are legal - king is not checked.
*/
struct Movegen {
    // Color, square
    static constexpr int MAX_LEGAL_MOVES = 218;
    static inline uint64_t PAWN_ATTACK_MOVES[2][64];
    static inline uint64_t PAWN_PUSH_MOVES[2][64];
    static inline uint64_t PAWN_ILLEGAL_AND[2][64];

    // Same for both colors.
    static inline uint64_t KNIGHT_MOVES[64];

    // Same for both colors.
    static inline uint64_t KING_MOVES[64];

    // used for "off range" move gen in bitboards.
    static inline uint64_t AND_BITBOARDS[64];

    // mask for piece checks - if its possible to make a castling (no pieces between)

    static inline uint64_t CASTLING_FREE_MASKS[2][2] = {{0xe00000000000000, 0x6000000000000000},{0xe,0x60}};


    static inline bool _initDone = false;

    /***
     * Initialization of all movegen tables - magic bitboards, knight bitboards, king bitboards.
     */
    static void init();

    static inline int _index = 0;
    static inline Move* _tmpMovesPtr;

    static inline uint64_t _all, _friendlyMerged, _enemyMerged;

    /***
     * Generates all possible pseudo-legal moves for a current position
     * @param board board
     * @param moves moves array to save all moves
     * @param capturesOnly for qsearch.
     * @return total number of moves, if king is checked.
     */
    static std::pair<int, bool> generateMoves(Board& board, Move* moves, bool capturesOnly = false);


    static bool validateKingCheck(int kingPos, bool whoPlay, uint64_t enemyBits[6]);

    static inline int PAWN_PUSH[] = {8,16};
    static inline int PAWN_ATTACKS[] = {7,9};
    /***
     * Pawn move generation
     * Attacks + normal moves
     * @param b pawn bitboard
     * @param enPassantSquare
     * @param color
     */
    static void generatePawnMoves(uint64_t b, int enPassantSquare, bool color);

    /***
     * ULL lookup tables for all king moves.
     */
    static inline int KING_OFFSETS[8] = {1,7,8,9,-1,-7,-8,-9};
    static void initKingMoves();

    /***
     * ULL Lookup tables for all squares for knights.
     */
    static inline int KNIGHT_OFFSETS[8] = {-17,-15,-10,-6,6,10,15,17};
    static void initKnightMoves();

    /***
     * Ull lookup for all pawn attacks.
     */
    static void initPawnAttacks();
    static void initPawnPushes();

    /***
     * Most idiotic initialization of AND-able bitboards for knight, king, pawn move generation.
     * Create maximally 4x4 area full of ones, and build bitboard.
     */
    static void initAndBitsForKKP();

    /***
     * Converts moveBitboard and appends them into a move vector
     * @param fromSquare bit _index from square
     * @param moveBitboard generated bitboard moves
     * @param pieceType
     * @note for all except pawns and kings. This method handles captures, quiets, checks, no more.
     */
    static void bitboardToMoves(int fromSquare, uint64_t& moveBitboard, Board::pieceType pieceType);

    static void generateRookMoves(uint64_t rooks);
    static void generateBishopMoves(uint64_t bishops);
    static void generateQueenMoves(uint64_t queens);
    static void generateKnightMoves(uint64_t knight);
    static void generateKingMoves(uint64_t king,  const std::array<bool, 2>& castling, bool whoPlay);
    static void generatePromotions(int fromSq, int toSq, bool capture);


    /*
        All capture generations.
    */
    static void generateRookCaptures(uint64_t rooks);
    static void generateBishopCaptures(uint64_t bishops);
    static void generateQueenCaptures(uint64_t queens);
    static void generateKnightCaptures(uint64_t knight);
    static void generateKingCaptures(uint64_t king);
    static void generatePawnCaptures(uint64_t b, int enPassantSquare, bool color);
    static void captureBitboardToMoves(int fromSquare, uint64_t& moveBitboard, Board::pieceType pieceType);
};


#endif //SENTINEL_MOVEGEN_H
