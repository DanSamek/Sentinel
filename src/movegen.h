#ifndef SENTINEL_MOVEGEN_H
#define SENTINEL_MOVEGEN_H

#include <vector>
#include <bitboard.h>
#include <magics.h>

/*
    TODO LIST
        -> king moves - DONE
        -> knight moves - DONE
        -> slider moves (use magic bitboards) DONE
            -> bishop DONE     TODO make "request to magic bitboard imp + collect moves"
            -> rook  DONE      TODO make "request to magic bitboard imp + collect moves"
                -> queen DONE  TODO make "request to magic bitboard imp + collect moves"

        -> pawn moves
            -> normal moves DONE
            -> en passant WHEN BOARD IMPLEMENTED

        -> pins
        -> checks
        -> king safe moves
        -> castling
        -> PERFT tests

*/
struct Movegen {
    // Color, square
    static inline uint64_t PAWN_MOVES[2][64];

    // Same for both colors.
    static inline uint64_t KNIGHT_MOVES[64];

    // Same for both colors.
    static inline uint64_t KING_MOVES[64];

    // used for "off range" move gen in bitboards.
    static inline uint64_t AND_BITBOARDS[64];

    /***
     * Initialization of all movegen tables - magic bitboards, knight bitboards, king bitboards.
     */
    static void initTables(){
        Magics::init();
        initAndBitsForKKP();
        initKnightMoves();
        initKingMoves();
    }

    static void generateMoves(){
        // check if king is checked.
            // do something.

        // get pinned pieces.
            // -> absolute - cant move
            // -> relative - can move in pinned direction.
                // -> pawns, rooks, bishops, queens.

        // bitboards iteration.
    }

    /***
     * Pawn move generation
     * Attacks + normal moves
     * EN passant TODO.
     * @param b pawn bitboard
     * @param occupancy entire occupancy
     */
    static void generatePawnMoves(Bitboard b, const Bitboard &current, const Bitboard &enemy, const Bitboard& all);

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
     * Most idiotic initialization of AND-able bitboards for knight, king, pawn move generation.
     * Create maximally 4x4 area full of ones, and build bitboard.
     */
    static void initAndBitsForKKP();
};


#endif //SENTINEL_MOVEGEN_H
