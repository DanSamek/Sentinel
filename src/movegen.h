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
        -> pins and checks
            -> precalculate direction table from each square (diagonal, horizontal)

        -> checks
            -> simple check detection. DONE
            -> get direction -> generate possible check blocks (!! knights !!) || king moves.
        -> pins
            -> XRays DONE
            -> & king bitboard
             if no -> continue;
            if yes -> find a piece if its hard pinned
                 -> knight hard pinned always.
                        or soft pinned.
                 -> if same direction as moves, we can generate em
                     -> move dir != attack dir -> hard pin.

        -> slider moves (use magic bitboards) DONE
            -> bishop DONE     TODO make "request to magic bitboard imp + collect moves"
            -> rook  DONE      TODO make "request to magic bitboard imp + collect moves"
                -> queen DONE  TODO make "request to magic bitboard imp + collect moves"

        -> pawn moves
            -> normal moves DONE
            -> en passant DONE
                -> one edge case on en passant !

        -> king safe moves DONE <=> we have enemy attacks.
        -> castling
        -> moveBitboard convertor for all pieces.
            -> DONE for all except pawns and kings.
        -> PERFT tests

*/
struct Movegen {
    // Color, square
    static inline uint64_t PAWN_ATTACK_MOVES[2][64];

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
        initPawnAttacks();
    }

    static std::pair<int, uint64_t> getKingChecksAndAttacks(const uint64_t& king, const std::vector<uint64_t>& enemyPieces, const uint64_t& all, bool enemyColor);

    static void generateMoves(const Board& board){
        auto friendlyBits = board.whoPlay ? board.whitePieces : board.blackPieces;
        auto enemyBits = board.whoPlay ? board.blackPieces : board.whitePieces;

        std::vector<Move> moves;

        uint64_t friendlyMerged = friendlyBits[0] | friendlyBits[1] | friendlyBits[2] | friendlyBits[3] | friendlyBits[4] | friendlyBits[5];
        uint64_t enemyMerged = enemyBits[0] | enemyBits[1] | enemyBits[2] | enemyBits[3] | enemyBits[4] | enemyBits[5];
        uint64_t all = friendlyMerged | enemyMerged;

        auto [enemyAttacks, checkCount] = getKingChecksAndAttacks(friendlyBits[Board::KING], enemyBits, all, !board.whoPlay);

        // if moves.count() == 0 => win for an enemy.
        if(checkCount == 2){
            // generate only king moves, not possible to defend 2 checks by a another pieces.
            return;
        }
        // now we will generate pinned pieces and directions.
        // auto pins = getPinsAndDirMasks(friendlyMerged, enemyQueens, enemyRooks, enemyBishops);


        if(checkCount == 1){
            // generate king moves and all possible blockers or captures of enemy attacker.
            return;
        }
        // if moves.count() == 0 => draw.
        // generate all possible moves.


        generatePawnMoves(friendlyBits[0], friendlyMerged, enemyMerged, all, board.enPassantSquare, false);
    }


    static inline int PAWN_PUSH[] = {8,16};
    static inline int PAWN_ATTACKS[] = {7,9};
    /***
     * Pawn move generation
     * Attacks + normal moves
     * EN passant TODO.
     * @param b pawn bitboard
     * @param occupancy entire occupancy
     */
    static void generatePawnMoves(uint64_t b, const uint64_t &current, const uint64_t &enemy, const uint64_t& all, int enPassantSquare, bool color);

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
    static uint64_t generateBishopPins(const uint64_t &enemyBishops, const uint64_t& enemy, const uint64_t &all, const uint64_t& currentKing);

    /***
     * Ray generation for pinned pieces.
     * @param rooks
     * @param all
     * @return
     */
    static uint64_t generateRookPins(const uint64_t &enemyRooks, const uint64_t& enemy, const uint64_t &all, const uint64_t& currentKing);

    /***
     * @param king
     * @param attacks
     * @param checkCount used in enemy movegeneration - count checks -> we can simplify movegen.
     * @return
     */
    static void incrementIfKingChecked(const uint64_t& king, const uint64_t& attacks, int& checkCount);

    /***
     * Converts moveBitboard and appends them into a move vector
     * @param fromSquare bit index from square
     * @param moveBitboard generated bitboard moves
     * @param moves reference to all moves -> will be returned from movegen.
     * @param enemies all bits for enemies
     * @param enemyKing bitboard for an enemy king
     * @param pieceType
     * Reason for this -> move order by checks/capture, etc..
     * @note for all except pawns and kings. This method handles captures, quiets, checks, no more.
     */
    static void bitboardToMoves(int fromSquare, uint64_t moveBitboard, std::vector<Move> &moves, const uint64_t& enemies, const uint64_t& enemyKing, Board::pieceType pieceType);
};


#endif //SENTINEL_MOVEGEN_H
