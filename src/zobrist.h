#ifndef SENTINEL_ZOBRIST_H
#define SENTINEL_ZOBRIST_H

#include <cstdint>
#include <ctime>
#include <cstdlib>
#include "magics.h"
#include "board.h"

class Zobrist {
    static constexpr int BOARD_SIZE = 64;
    static constexpr int NUM_PIECES = 12;
    static constexpr int NUM_FILES = 8;
    static constexpr int NUM_CASTLING = 16;
    static inline uint64_t zobristTable[NUM_PIECES][BOARD_SIZE];
    static inline uint64_t castlingTable[NUM_CASTLING];
public:
    static inline uint64_t sideToMove;
    static inline uint64_t noEnPassant;
    static inline uint64_t enPassantTable[NUM_FILES];
    inline static void init(){
        // all squares
        for(int piece = 0; piece < NUM_PIECES; piece++){
            for(int square = 0; square < BOARD_SIZE; square++){
                zobristTable[piece][square] = Magics::randUInt64();
            }
        }

        // lazy to figure out enpassant for each side.
        for(int i = 0; i < NUM_FILES; i++){
            enPassantTable[i] = Magics::randUInt64();
        }

        for(int i = 0; i < NUM_CASTLING; i++){
            castlingTable[i] = Magics::randUInt64();
        }
        sideToMove = Magics::randUInt64();
        noEnPassant = Magics::randUInt64();
    }

    static inline void updateHashMove(uint64_t& hash, const Move& move, const Board& board, const State& state){
        int fromSquare = move.fromSq;
        int toSquare = move.toSq;
        auto piece = move.movePiece + (board.whoPlay ? 0 : 6);

        if(move.moveType == Move::CAPTURE){
            auto capturedPiece = state.captureType + (board.whoPlay ? 6 : 0);
            hash ^= zobristTable[capturedPiece][toSquare];
        }

        hash ^= zobristTable[piece][fromSquare];
        hash ^= zobristTable[piece][toSquare];

        if(move.moveType == Move::DOUBLE_PAWN_UP){
            hash ^= enPassantTable[board.enPassantSquare % 8];
        }

        // capture
        if(move.toSq == 0 || move.toSq == 7 || move.toSq == 63 || move.toSq == 56 ||
        // rook moves.
            (move.movePiece == Board::ROOK  && (move.fromSq == 0 || move.fromSq == 7 || move.fromSq == 63 || move.fromSq == 56))){
            updateCastlingRightsHash(hash, board, state);
        }
        hash ^= sideToMove;
    }

    static inline void updateCastlingHash(uint64_t& hash, const Move& move, const Board& board, const State& state){
        int king = board.whoPlay ? Board::KING : (Board::KING + 6);
        int rook = board.whoPlay ? Board::ROOK : (Board::ROOK + 6);
        // king XOR from position
        hash ^= zobristTable[king][move.fromSq];
        // XOR rook from position
        hash ^= zobristTable[rook][move.fromSq + (move.toSq > move.fromSq ? 3 : -4)];

        // XOR RIGHT CASTLING.
        // KING SIDE
        hash ^= zobristTable[king][move.toSq];
        if(move.toSq > move.fromSq) hash ^= zobristTable[rook][move.toSq - 1];
        // QUEEN SIDE.
        else hash ^= zobristTable[rook][move.toSq + 1];

        // castling rights.
        updateCastlingRightsHash(hash, board, state);

        hash ^= sideToMove;
    }

    static inline void updateEnPassantHash(uint64_t& hash, const Move& move, const Board& board, const State& state){
        int piece = move.movePiece + (board.whoPlay ? 0 : 6);
        int capturedPiece = (board.whoPlay ? 6 : 0);

        // remove en-passant hash
        hash ^= enPassantTable[state.enPassantSquare % 8];
        hash ^= noEnPassant;

        // normal capture
        hash ^= zobristTable[capturedPiece][move.toSq + (board.whoPlay ? +8 : -8)];

        // XOR from
        hash ^= zobristTable[piece][move.fromSq];

        // XOR to
        hash ^= zobristTable[piece][move.toSq];

        hash ^= sideToMove;
    }

    static inline void updateCastlingRightsHash(uint64_t& hash, const Board& board, const State& state, bool force = false){
        // WHITE, BLACK {queen, king}
        // << 3
        // << 2
        // << 1
        // << 0
        if(state.castling == board.castling && !force) return; // Dont xor same castlings!

        uint prevCastling = (uint)state.castling[0][0] << 3 | (uint)state.castling[0][1] << 2 | (uint)state.castling[1][0] << 1 | (uint)state.castling[1][1] << 0;
        hash ^= castlingTable[prevCastling];

        uint castling = (uint)board.castling[0][0] << 3 | (uint)board.castling[0][1] << 2 | (uint)board.castling[1][0] << 1 | (uint)board.castling[1][1] << 0;
        hash ^= castlingTable[castling];

    }

    static uint64_t getHash(const Board& board){
        uint64_t hash = 0ULL;

        sideXORing(board.whitePieces, hash, 0);
        sideXORing(board.blackPieces, hash, 6);

        // black to move.
        if(!board.whoPlay) hash ^= sideToMove;

        // en passant square
        if(board.enPassantSquare != -1) hash ^= enPassantTable[board.enPassantSquare % 8];

        uint castling = (uint)board.castling[0][0] << 3 | (uint)board.castling[0][1] << 2 | (uint)board.castling[1][0] << 1 | (uint)board.castling[1][1] << 0;
        hash ^= castlingTable[castling];
        return hash;
    }

    static inline void updatePromotionHash(uint64_t& hash, const Move& move, const Board& board, const State& state){
        int addition = (board.whoPlay ? 0 : 6);
        int piece =  move.movePiece + addition;
        int targetPiece = move.promotionType + addition;
        // XOR from sq
        hash ^= zobristTable[piece][move.fromSq];
        // Move pawn, but xor with target piece
        hash ^= zobristTable[targetPiece][move.toSq];

        // can be capture
        if(state.captureType != -1){
            int enemyAddition = board.whoPlay ? 6 : 0;
            hash ^= zobristTable[state.captureType + enemyAddition][move.toSq];
        }
        // !! castling problems
        if(move.toSq == 0 || move.toSq == 7 || move.toSq == 63 || move.toSq == 56){
            updateCastlingRightsHash(hash, board, state);
        }
        hash ^= sideToMove;
    }

private:
    static inline void sideXORing(const uint64_t *bbs, uint64_t& hash, int mover) {
        for(int j = 0; j < 6; j++){
            auto bb = bbs[j];
            while(bb){
                int pos = bit_ops::bitScanForwardPopLsb(bb);
                hash ^= zobristTable[j+mover][pos];
            }
        }
    }
};


#endif //SENTINEL_ZOBRIST_H
