#ifndef SENTINEL_MOVEGEN_H
#define SENTINEL_MOVEGEN_H

#include <vector>
#include <magics.h>
#include <move.h>
#include <board.h>
#include <span>

#define UPDATE_BOARD_STATE(board, whoPlay) \
    auto friendlyBits = whoPlay ? board.whitePieces : board.blackPieces; \
    auto enemyBits = whoPlay ? board.blackPieces : board.whitePieces; \
    Movegen::friendlyMerged = friendlyBits[0] | friendlyBits[1] | friendlyBits[2] | friendlyBits[3] | friendlyBits[4] | friendlyBits[5]; \
    Movegen::enemyMerged = enemyBits[0] | enemyBits[1] | enemyBits[2] | enemyBits[3] | enemyBits[4] | enemyBits[5]; \
    Movegen::all = Movegen::friendlyMerged | Movegen::enemyMerged

#define VALIDATE_KING_CHECKS(kingPos, board, tmpMoves, j, enemyBits) \
    bool valid = Movegen::validateKingCheck(kingPos, board.whoPlay, enemyBits); \
    if (!valid) continue; \
    bool kingSide = tmpMoves[j].toSq > tmpMoves[j].fromSq; \
    int sqToCheck = kingSide ? tmpMoves[j].fromSq + 1 : tmpMoves[j].fromSq - 1; \
    valid = Movegen::validateKingCheck(sqToCheck, board.whoPlay, enemyBits); \
    if (!valid) continue; \
    sqToCheck = kingSide ? tmpMoves[j].fromSq + 2 : tmpMoves[j].fromSq - 2; \
    valid = Movegen::validateKingCheck(sqToCheck, board.whoPlay, enemyBits); \
    if (!valid) continue


/*
    !!! DOING PSEUDO LEGAL MOVE GENERATION !!!
        AKA generate all moves, check if moves are legal - king not checked.
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
    static void initTables(){
        if(_initDone) return;
        Magics::init();
        initAndBitsForKKP();
        initKnightMoves();
        initKingMoves();
        initPawnAttacks();
        initPawnPushes();
        _initDone = true;
    }

    static inline int index = 0;

    static inline  Move tmpMoves[MAX_LEGAL_MOVES];
    static inline Move* tmpMovesPtr;

    static inline uint64_t all, friendlyMerged, enemyMerged;

    static bool validateKingCheck(int kingPos, bool whoPlay, uint64_t enemyBits[6]);

    /***
     * @param board Board for movegen.
     * @param moves Result valid moves.
     * @return totalNumber of moves + if king is checked.
     */
    static std::pair<int, bool> generateMoves(Board& board, Move* moves, bool legalOnly = true){
        index = 0;
        // movegen ptrs.
        if(legalOnly) tmpMovesPtr = tmpMoves;
        else tmpMovesPtr = moves;

        UPDATE_BOARD_STATE(board, board.whoPlay);

        auto kingPos = bit_ops::bitScanForward(friendlyBits[Board::KING]);
        // if its not valid <-> checked king !!!
        bool checked = ! validateKingCheck(kingPos, !board.whoPlay, enemyBits);

        // generate all possible moves for current player.
        generatePawnMoves(friendlyBits[Board::PAWN], board.enPassantSquare, board.whoPlay);
        generateRookMoves(friendlyBits[Board::ROOK]);
        generateBishopMoves(friendlyBits[Board::BISHOP]);
        generateQueenMoves(friendlyBits[Board::QUEEN]);
        generateKnightMoves(friendlyBits[Board::KNIGHT]);
        generateKingMoves(friendlyBits[Board::KING], board.castling[!board.whoPlay], board.whoPlay);

        // copy to a result, if we want all pseudolegal moves <-> we will check it in search/perft (just a performance try).
        if(!legalOnly) return {index, checked};

        int resultSize = 0;
        for(int j = 0; j < index; j++){
            // castling move, check 2 another squares.
            auto kingPos = bit_ops::bitScanForward(friendlyBits[Board::KING]);
            if(tmpMovesPtr[j].moveType == Move::CASTLING){
                UPDATE_BOARD_STATE(board, board.whoPlay);
                VALIDATE_KING_CHECKS(kingPos, board, tmpMovesPtr, j, enemyBits);
            }
            // play move.
            board.makeMove(tmpMovesPtr[j], false);
            // we need updated pieces.
            // !! changed move !! (whoplay).
            UPDATE_BOARD_STATE(board, !board.whoPlay);
            kingPos = bit_ops::bitScanForward(friendlyBits[Board::KING]);

            auto valid = validateKingCheck(kingPos, !board.whoPlay, enemyBits);
            if(valid){
                moves[resultSize] = std::move(tmpMovesPtr[j]);
                resultSize++;
            }
            board.undoMove(tmpMovesPtr[j], false);
        }

        return {resultSize, checked};
    }


    static inline int PAWN_PUSH[] = {8,16};
    static inline int PAWN_ATTACKS[] = {7,9};
    /***
     * Pawn move generation
     * Attacks + normal moves
     * @param b pawn bitboard
     * @param occupancy entire occupancy
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
     * @param enemyMerged all bits for enemyMerged
     * @param enemyKing bitboard for an enemy king
     * @param pieceType
     * Reason for this -> move order by checks/capture, etc..
     * @note for all except pawns and kings. This method handles captures, quiets, checks, no more.
     */
    static void bitboardToMoves(int fromSquare, uint64_t& moveBitboard, Board::pieceType pieceType);

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
    static void generateRookMoves(uint64_t rooks);
    static void generateBishopMoves(uint64_t bishops);
    static void generateQueenMoves(uint64_t queens);
    static void generateKnightMoves(uint64_t knight);
    static void generateKingMoves(uint64_t king,  const std::array<bool, 2>& castling, bool whoPlay);
    static void generatePromotions(int fromSq, int toSq);
};


#endif //SENTINEL_MOVEGEN_H
