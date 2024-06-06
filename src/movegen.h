#ifndef SENTINEL_MOVEGEN_H
#define SENTINEL_MOVEGEN_H

#include <vector>
#include <bitboard.h>
#include <magics.h>
#include <board.h>
#include "move.h"

/*
    TODO LIST
        -> king moves - DONE
        -> knight moves - DONE

        PRIORITY
        -> pins - generate rays, for sliding pieces [64][4] and rays.
        -> checks

        -> slider moves (use magic bitboards) DONE
            -> bishop DONE     TODO make "request to magic bitboard imp + collect moves"
            -> rook  DONE      TODO make "request to magic bitboard imp + collect moves"
                -> queen DONE  TODO make "request to magic bitboard imp + collect moves"

        -> pawn moves
            -> normal moves DONE
            -> en passant WHEN BOARD IMPLEMENTED

        -> king safe moves
        -> castling
        -> PERFT tests

*/
struct Movegen {
    // Color, square
    static inline Bitboard PAWN_MOVES[2][64];

    // Same for both colors.
    static inline Bitboard KNIGHT_MOVES[64];

    // Same for both colors.
    static inline Bitboard KING_MOVES[64];

    // used for "off range" move gen in bitboards.
    static inline Bitboard AND_BITBOARDS[64];

    /***
     * Initialization of all movegen tables - magic bitboards, knight bitboards, king bitboards.
     */
    static void initTables(){
        Magics::init();
        initAndBitsForKKP();
        initKnightMoves();
        initKingMoves();
    }

    static void generateMoves(const Board& board){
        auto friendlyBits = board.whoPlay ? board.whitePieces : board.blackPieces;
        auto enemyBits = board.whoPlay ? board.blackPieces : board.whitePieces;

        std::vector<Move> moves;

        Bitboard friendlyMerged = friendlyBits[0] | friendlyBits[1] | friendlyBits[2] | friendlyBits[3] | friendlyBits[4] | friendlyBits[5];
        Bitboard enemyMerged = enemyBits[0] | enemyBits[1] | enemyBits[2] | enemyBits[3] | enemyBits[4] | enemyBits[5];
        Bitboard all = friendlyMerged | enemyMerged;

        // check if king is checked.
            // do something.

        // get pinned pieces.
            // -> absolute - cant move
            // -> relative - can move in pinned direction.
                // -> pawns, rooks, bishops, queens.


        // generate moves.
        generatePawnMoves(friendlyBits[0], friendlyMerged, enemyMerged, all, board.enPassantSquare);
    }

    /***
     * Pawn move generation
     * Attacks + normal moves
     * EN passant TODO.
     * @param b pawn bitboard
     * @param occupancy entire occupancy
     */
    static void generatePawnMoves(Bitboard b, const Bitboard &current, const Bitboard &enemy, const Bitboard& all, int enPassantSquare);

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

    /***
     * Ray generation for pinned pieces.
     * @param bishops
     * @param all
     * @return
     */
    static Bitboard generateBishopPins(const Bitboard &enemyBishops, const Bitboard& enemy, const Bitboard &all, const Bitboard& currentKing);

    /***
     * Ray generation for pinned pieces.
     * @param rooks
     * @param all
     * @return
     */
    static Bitboard generateRookPins(const Bitboard &enemyRooks, const Bitboard& enemy, const Bitboard &all, const Bitboard& currentKing);
};


#endif //SENTINEL_MOVEGEN_H
