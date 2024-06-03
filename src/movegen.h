#ifndef SENTINEL_MOVEGEN_H
#define SENTINEL_MOVEGEN_H

#include <vector>
#include <bitboard.h>


/*
    TODO LIST
        -> pawn moves
            -> en passant
            -> normal moves
        -> king moves - DONE
        -> knight moves - DONE
        -> slider moves (use magic bitboards)
            -> bishop
            -> rook
                -> queen

        After simple moves
        -> checks
        -> pins
        -> castling
        -> king safe moves
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
     * Initialization of all movegen tables - magic bitboards, knight bitboards, pawn bitboards.
     */
    static void initTables(){
        initAndBitsForKKP();
        initKnightMoves();
        initKingMoves();
        initPawnMoves();
    }

    static void generateMoves(const std::vector<Bitboard>& currentBitboards, const std::vector<Bitboard>& enemyBitboards){
        // TODO.
    }

    /***
     * ULL lookup tables for all squares for pawns.
     */
    static void initPawnMoves(){

    }

    /***
     * ULL lookup tables for all king moves.
     */
    static inline  int KING_OFFSETS[8] = {1,7,8,9,-1,-7,-8,-9};
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
