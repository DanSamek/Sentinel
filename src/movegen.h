#ifndef SENTINEL_MOVEGEN_H
#define SENTINEL_MOVEGEN_H

#include <vector>
#include <bitboard.h>
#include <magics.h>
#include <board.h>
#include "move.h"

/*

    !!! DOING PSEUDO LEGAL MOVE GENERATION !!!
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
    static inline uint64_t PAWN_PUSH_MOVES[2][64];
    static inline uint64_t PAWN_ILLEGAL_AND[2][64];

    // Same for both colors.
    static inline uint64_t KNIGHT_MOVES[64];

    // Same for both colors.
    static inline uint64_t KING_MOVES[64];

    // used for "off range" move gen in bitboards.
    static inline uint64_t AND_BITBOARDS[64];

    // mask for piece checks - if its possible to make a castling (no pieces between)
    static inline uint64_t CASTLING_FREE_MASKS[2][2] = {{0x6000000000000000, 0x600000000000000},{0x6,0x60}};
    /***
     * Initialization of all movegen tables - magic bitboards, knight bitboards, king bitboards.
     */
    static void initTables(){
        Magics::init();
        initAndBitsForKKP();
        initKnightMoves();
        initKingMoves();
        initPawnAttacks();
        initPawnPushes();
        tmpMoves.resize(218); // max possible moves.
    }

    static inline int index = 0;

    /***
     * Generates enemy constant attacks - king, knights, pawns.
     * @param king bitboard
     * @param enemyPieces
     * @param enemyColor true -> white, false -> black.
     * @return attack bitboard.
     */
    static uint64_t generateConstantEnemyAttacks(const uint64_t& king, const std::vector<uint64_t>& enemyPieces, bool enemyColor);

    static inline std::vector<Move> tmpMoves, resultMoves;

    static std::vector<Move> generateMoves(const Board& board){
        resultMoves.clear();
        index = 0;
        auto friendlyBits = board.whoPlay ? board.whitePieces : board.blackPieces;
        auto enemyBits = board.whoPlay ? board.blackPieces : board.whitePieces;
        uint64_t friendlyMerged = friendlyBits[0] | friendlyBits[1] | friendlyBits[2] | friendlyBits[3] | friendlyBits[4] | friendlyBits[5];
        uint64_t enemyMerged = enemyBits[0] | enemyBits[1] | enemyBits[2] | enemyBits[3] | enemyBits[4] | enemyBits[5];
        uint64_t all = friendlyMerged | enemyMerged;

        // generate enemy king, pawn, knight moves -> constant bitboard of enemy attacks.
        auto constantEnemyBB = generateConstantEnemyAttacks(friendlyBits[Board::KING], enemyBits, !board.whoPlay);

        // generate all possible moves for current player.
        generatePawnMoves(friendlyBits[Board::PAWN], friendlyMerged, enemyMerged, all, board.enPassantSquare, board.whoPlay, tmpMoves);
        generateRookMoves(friendlyBits[Board::ROOK], friendlyMerged, enemyMerged, all, tmpMoves, enemyBits[Board::KING]);
        generateBishopMoves(friendlyBits[Board::BISHOP], friendlyMerged, enemyMerged, all, tmpMoves, enemyBits[Board::KING]);
        generateQueenMoves(friendlyBits[Board::QUEEN], friendlyMerged, enemyMerged, all, tmpMoves, enemyBits[Board::KING]);
        generateKnightMoves(friendlyBits[Board::KNIGHT], friendlyMerged, enemyMerged, all, tmpMoves, enemyBits[Board::KING]);
        generateKingMoves(friendlyBits[Board::KING], friendlyMerged, enemyMerged, all, tmpMoves, board.castling[!board.whoPlay], board.whoPlay);

        // generate enemy attacks
        // play them on a board, check if there is no enemy check <-> handle current king as a queen, check all rays, if somewhere is slider | constantEb -> not a valid move.
        return resultMoves;
    }


    static inline int PAWN_PUSH[] = {8,16};
    static inline int PAWN_ATTACKS[] = {7,9};
    /***
     * Pawn move generation
     * Attacks + normal moves
     * @param b pawn bitboard
     * @param occupancy entire occupancy
     */
    static void generatePawnMoves(uint64_t b, const uint64_t &current, const uint64_t &enemy, const uint64_t& all, int enPassantSquare, bool color, std::vector<Move>& moves);

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
    static void bitboardToMoves(int fromSquare, uint64_t moveBitboard, std::vector<Move> &moves, const uint64_t& enemies, const uint64_t& enemyKing, Board::pieceType pieceType, const uint64_t& friendly);

    /***
     * Slider move generation. for  generateRookMoves, generateBishopMoves, generateQueenMoves
     * @param piece
     * @param pieceType
     * @param rook
     * @param friendlyMerged
     * @param enemyMerged
     * @param all
     * @param moves
     * @param enemyKing
     */
    static void generateRookMoves(uint64_t rooks, const uint64_t& friendlyMerged, const uint64_t& enemyMerged, const uint64_t& all, std::vector<Move> &moves, const uint64_t& enemyKing);
    static void generateBishopMoves(uint64_t bishops, const uint64_t& friendlyMerged, const uint64_t& enemyMerged, const uint64_t& all, std::vector<Move> &moves, const uint64_t& enemyKing);
    static void generateQueenMoves(uint64_t queens, const uint64_t& friendlyMerged, const uint64_t& enemyMerged, const uint64_t& all, std::vector<Move> &moves, const uint64_t& enemyKing);
    static void generateKnightMoves(uint64_t knight, const uint64_t& friendlyMerged, const uint64_t& enemyMerged, const uint64_t& all, std::vector<Move> &moves, const uint64_t& enemyKing);
    static void generateKingMoves(uint64_t king, const uint64_t& friendlyMerged, const uint64_t& enemyMerged, const uint64_t& all, std::vector<Move> &moves, const bool castling[2], bool whoPlay);

};


#endif //SENTINEL_MOVEGEN_H
