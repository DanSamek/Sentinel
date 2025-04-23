#ifndef SENTINEL_MOVEGEN_H
#define SENTINEL_MOVEGEN_H

#include <vector>
#include "magics.h"
#include "move.h"
class Board;
#include "board.h"
#include <span>


namespace Sentinel{
    /*
        !!! DOING PSEUDO LEGAL MOVE GENERATION !!!
            AKA generate all moves, check if moves are legal - king is not checked.
    */
    struct Movegen {
        Movegen(Board& board, Move* moves);

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

        int index = 0;
        Board& board;
        Move* movesPtr;

        uint64_t all, friendlyMerged, enemyMerged;
        uint64_t *friendlyBits;
        uint64_t *enemyBits;


        template <bool capturesOnly>
        std::pair<int, bool> generateMoves(){
            auto kingPos = bit_ops::bitScanForward(friendlyBits[KING]);

            bool checked = !validateKingCheck(kingPos);
            if (capturesOnly){
                if(board.whoPlay) generatePawnCaptures<true>(friendlyBits[PAWN]);
                else generatePawnCaptures<false>(friendlyBits[PAWN]);

                generateRookCaptures(friendlyBits[ROOK]);
                generateBishopCaptures(friendlyBits[BISHOP]);
                generateQueenCaptures(friendlyBits[QUEEN]);
                generateKnightCaptures(friendlyBits[KNIGHT]);
                generateKingCaptures(friendlyBits[KING]);
                return {index, checked};
            }

            if(board.whoPlay) generatePawnMoves<true>(friendlyBits[PAWN], board.enPassantSquare);
            else generatePawnMoves<false>(friendlyBits[PAWN], board.enPassantSquare);

            generateRookMoves(friendlyBits[ROOK]);
            generateBishopMoves(friendlyBits[BISHOP]);
            generateQueenMoves(friendlyBits[QUEEN]);
            generateKnightMoves(friendlyBits[KNIGHT]);
            generateKingMoves(friendlyBits[KING], board.castling[!board.whoPlay]);

            return {index, checked};
        }


        bool validateKingCheck(int kingPos);

        static inline int PAWN_PUSH[] = {8,16};
        static inline int PAWN_ATTACKS[] = {7,9};
        /***
         * Pawn move generation
         * Attacks + normal moves
         * @param b pawn bitboard
         * @param enPassantSquare
         */
        template<bool color>
        void generatePawnMoves(uint64_t b, int enPassantSquare){
            uint64_t enPassantBB = enPassantSquare != -1 ? 1ULL << enPassantSquare : 0;
            constexpr auto tmpColor = (PIECE_COLOR)!color;
            while(b){
                int bit = bit_ops::bitScanForwardPopLsb(b);
                int rank = bit / 8;
                bool promotion = (tmpColor == WHITE && rank == 1) ||(tmpColor == BLACK && rank == 6); // next move will be promotion on 100%!

                auto bb = PAWN_PUSH_MOVES[!color][bit];
                if(((tmpColor == WHITE && rank == 6) || (tmpColor == BLACK && rank == 1)) && (bb & all) != PAWN_ILLEGAL_AND[!color][bit]){
                    bb &= ~all;
                    while(bb){
                        auto tmpBit = bit_ops::bitScanForwardPopLsb(bb);
                        movesPtr[index++] = {bit, tmpBit, Move::NONE, abs(tmpBit - bit) > 8 ? Move::DOUBLE_PAWN_UP : Move::QUIET, PIECE_TYPE::PAWN};
                    }
                }
                else{
                    while(!(bb & all) && bb){
                        auto tmpBit = bit_ops::bitScanForwardPopLsb(bb);
                        if(promotion) generatePromotions(bit, tmpBit, false);
                        else movesPtr[index++] = {bit, tmpBit, Move::NONE, Move::QUIET, PIECE_TYPE::PAWN};
                    }
                }


                bb = PAWN_ATTACK_MOVES[!color][bit] & (enemyMerged | enPassantBB);
                // normal captures || captures with promotions.
                while(bb){
                    auto tmpBit = bit_ops::bitScanForwardPopLsb(bb);
                    if(tmpBit == enPassantSquare) movesPtr[index++] = {bit, tmpBit, Move::NONE, Move::EN_PASSANT, PIECE_TYPE::PAWN};
                    else if(promotion) generatePromotions(bit, tmpBit, true);
                    else movesPtr[index++] = {bit, tmpBit, Move::NONE, Move::CAPTURE, PIECE_TYPE::PAWN};
                }
            }
        }

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
         * @param fromSquare bit index from square
         * @param moveBitboard generated bitboard moves
         * @param pieceType
         * @note for all except pawns and kings. This method handles captures, quiets, checks, no more.
         */
        void bitboardToMoves(int fromSquare, uint64_t& moveBitboard, PIECE_TYPE pieceType);

        void generateRookMoves(uint64_t rooks);
        void generateBishopMoves(uint64_t bishops);
        void generateQueenMoves(uint64_t queens);
        void generateKnightMoves(uint64_t knight);
        void generateKingMoves(uint64_t king,  const std::array<bool, 2>& castling);
        void generatePromotions(int fromSq, int toSq, bool capture);


        /*
            All capture generations.
        */
        void generateRookCaptures(uint64_t rooks);
        void generateBishopCaptures(uint64_t bishops);
        void generateQueenCaptures(uint64_t queens);
        void generateKnightCaptures(uint64_t knight);
        void generateKingCaptures(uint64_t king);

        template<bool color>
        void generatePawnCaptures(uint64_t b) {
            uint64_t enPassantBB = board.enPassantSquare != -1 ? 1ULL << board.enPassantSquare : 0;
            constexpr auto tmpColor = !color;
            while(b){
                int bit = bit_ops::bitScanForwardPopLsb(b);
                int rank = bit / 8;
                bool promotion = (tmpColor == PIECE_COLOR::WHITE && rank == 1) ||(tmpColor == PIECE_COLOR::BLACK && rank == 6); // next move will be promotion !!

                auto bb = PAWN_ATTACK_MOVES[tmpColor][bit] & (enemyMerged | enPassantBB);
                while(bb){
                    auto tmpBit = bit_ops::bitScanForwardPopLsb(bb);
                    if(tmpBit == board.enPassantSquare) movesPtr[index++] = {bit, tmpBit, Move::NONE, Move::EN_PASSANT, PIECE_TYPE::PAWN};
                    else if(promotion) generatePromotions(bit, tmpBit, true);
                    else movesPtr[index++] = {bit, tmpBit, Move::NONE, Move::CAPTURE, PIECE_TYPE::PAWN};
                }
            }
        }


        void captureBitboardToMoves(int fromSquare, uint64_t& moveBitboard, PIECE_TYPE pieceType);
    };

}
#endif //SENTINEL_MOVEGEN_H
