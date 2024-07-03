#ifndef SENTINEL_PST_H
#define SENTINEL_PST_H

#include <cassert>

struct PST{
    static constexpr int PIECE_EVAL_MG[6] = {100, 305, 325, 500, 900, 10000};
    static constexpr int PIECE_EVAL_EG[6] = {115, 290, 340, 550, 1000, 10000};


    static constexpr inline int MG_PAWN_TABLE[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            -10, -10, -10, -10, -10, -10, -10, -10,
            -10, -5, -5, -5, -5, -5, -5, -10,
            -5, 0, 0, 5, 5, 0, 0, -5,
            -5, 0, 10, 20, 20, 10, 0, -5,
            0, 5, 10, 25, 25, 10, 5, 0,
            5, 10, 20, 30, 30, 20, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0,
    };


    static constexpr inline int EG_PAWN_TABLE[64] = {
            0,    0,    0,    0,    0,    0,    0,    0,
            65, 65, 65, 65, 65, 65, 65, 65,
            47, 47, 47, 47, 47, 47, 47, 47,
            35, 35, 35, 35, 35, 35, 35, 35,
            25, 25, 25, 25, 25, 25, 25, 25,
            15, 15, 15, 15, 15, 15, 15, 15,
            5, 5, 5, 5, 5, 5, 5, 5,
            0,    0,    0,    0,    0,    0,    0,    0
    };

    static constexpr inline int MG_KNIGHT_TABLE[64] = {
            -50, -40, -30, -30, -30, -30, -40, -50,
            -40, -20, 0, 5, 5, 0, -20, -40,
            -30, 5, 10, 15, 15, 10, 5, -30,
            -30, 0, 15, 20, 20, 15, 0, -30,
            -30, 5, 15, 20, 20, 15, 5, -30,
            -30, 0, 10, 15, 15, 10, 0, -30,
            -40, -20, 0, 0, 0, 0, -20, -40,
            -50, -40, -30, -30, -30, -30, -40, -50,
    };

    static constexpr inline int EG_KNIGHT_TABLE[64] = {
            -40, -30, -20, -20, -20, -20, -30, -40,
            -30, -10, 0, 5, 5, 0, -10, -30,
            -20, 5, 10, 15, 15, 10, 5, -20,
            -20, 0, 15, 20, 20, 15, 0, -20,
            -20, 5, 15, 20, 20, 15, 5, -20,
            -20, 0, 10, 15, 15, 10, 0, -20,
            -30, -10, 0, 0, 0, 0, -10, -30,
            -40, -30, -20, -20, -20, -20, -30, -40,
    };

    static constexpr inline int MG_BISHOP_TABLE[64] = {
            -20, -10, -10, -10, -10, -10, -10, -20,
            -10, 0, 0, 0, 0, 0, 0, -10,
            -10, 0, 0, 0, 0, 0, 0, -10,
            -10, 5, 5, 10, 10, 5, 5, -10,
            -10, 0, 10, 10, 10, 10, 0, -10,
            -10, 10, 10, 10, 10, 10, 10, -10,
            -10, 5, 0, 0, 0, 0, 5, -10,
            -20, -10, -10, -10, -10, -10, -10, -20,
    };

    static constexpr inline int EG_BISHOP_TABLE[64] = {
            -20, -10, -10, -10, -10, -10, -10, -20,
            -10, 5, 10, 10, 10, 10, 5, -10,
            -10, 10, 10, 15, 15, 10, 10, -10,
            -10, 10, 15, 20, 20, 15, 10, -10,
            -10, 10, 15, 20, 20, 15, 10, -10,
            -10, 10, 10, 15, 15, 10, 10, -10,
            -10, 5, 10, 10, 10, 10, 5, -10,
            -20, -10, -10, -10, -10, -10, -10, -20,
    };


    static constexpr inline int MG_ROOK_TABLE[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            5, 10, 10, 10, 10, 10, 10, 5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            0, 0, 0, 5, 5, 0, 0, 0,
    };

    static constexpr inline int EG_ROOK_TABLE[64] = {
            0, 0, 0, 5, 5, 0, 0, 0,
            5, 10, 10, 10, 10, 10, 10, 5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            -5, 0, 0, 0, 0, 0, 0, -5,
            0, 0, 0, 5, 5, 0, 0, 0,
    };

    static constexpr inline int MG_QUEEN_TABLE[64] = {
            -20, -10, -10, -5, -5, -10, -10, -20,
            -10, 0, 0, 0, 0, 0, 0, -10,
            -10, 0, 5, 0, 0, 5, 0, -10,
            -10, 5, 5, 5, 5, 5, 0, -10,
            -5, 0, 5, 5, 5, 5, 0, -5,
            -10, 0, 5, 5, 5, 5, 0, -10,
            -10, 0, 0, 0, 0, 0, 0, -10,
            -20, -10, -10, -5, -5, -10, -10, -20,
    };

    static constexpr inline int EG_QUEEN_TABLE[64] = {
            -20, -10, -10, -5, -5, -10, -10, -20,
            -10, 0, 0, 0, 0, 0, 0, -10,
            -10, 0, 5, 0, 0, 5, 0, -10,
            -10, 5, 5, 5, 5, 5, 0, -10,
            -5, 0, 5, 5, 5, 5, 0, -5,
            -10, 0, 5, 5, 5, 5, 0, -10,
            -10, 0, 0, 0, 0, 0, 0, -10,
            -20, -10, -10, -5, -5, -10, -10, -20,
    };

    static constexpr inline int MG_KING_TABLE[64] = {
            -40,  -40,  -40,  -40,  -40,  -40,  -40,  -40,
            -30,  -30,  -30,  -30,  -30,  -30,  -30,  -30,
            -20,  -20,  -20,  -20,  -20,  -20,  -20,  -20,
            -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,
            -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,
            -8,   -8,   -15,  -25,  -25,  -15,  -8,   -8,
            24,    7,    -7,  -15,  -15,   -7,   7,   24,
            33,   43,    27,   0,   0,   27,   43,   33
    };

    static constexpr inline int EG_KING_TABLE[64] = {
            -50, -40, -30, -30, -30, -30, -40, -50,
            -40, -20, 0, 0, 0, 0, -20, -40,
            -30, 0, 10, 15, 15, 10, 0, -30,
            -30, 0, 15, 20, 20, 15, 0, -30,
            -30, 0, 15, 20, 20, 15, 0, -30,
            -30, 0, 10, 15, 15, 10, 0, -30,
            -40, -20, 0, 0, 0, 0, -20, -40,
            -50, -40, -30, -30, -30, -30, -40, -50,

    };

    const static inline int* MG_PST[6] =
    {
            MG_PAWN_TABLE,
            MG_KNIGHT_TABLE,
            MG_BISHOP_TABLE,
            MG_ROOK_TABLE,
            MG_QUEEN_TABLE,
            MG_KING_TABLE
    };

    const static inline int* EG_PST[6] =
    {
            EG_PAWN_TABLE,
            EG_KNIGHT_TABLE,
            EG_BISHOP_TABLE,
            EG_ROOK_TABLE,
            EG_QUEEN_TABLE,
            EG_KING_TABLE
    };


    #define FLIP(sq) ((sq)^56)

    static inline int MG_TABLE[12][64];
    static inline int EG_TABLE[12][64];

    static void init() {
        // +6 for black side.
        for (int j = 0; j < 6; j++) {
            for (int square = 0; square < 64; square++) {
                // white side.
                MG_TABLE[j][square] = PIECE_EVAL_MG[j] + MG_PST[j][square];
                EG_TABLE[j][square] = PIECE_EVAL_EG[j] + EG_PST[j][square];
                // black side.
                MG_TABLE[j + 6][square] = PIECE_EVAL_MG[j] + MG_PST[j][FLIP(square)];
                EG_TABLE[j + 6][square] = PIECE_EVAL_EG[j] + EG_PST[j][FLIP(square)];
            }
        }
    }


    /***
     * Gets PST value for current piece.
     * @param color white = true, black = false
     * @param pieceType
     * @param square
     * @param endgame if false => middlegame.
     * @return
     */
    static inline int getValue(bool color, int pieceType, int square, bool endgame){
        pieceType += color ? 0 : 6;
        return endgame ? EG_TABLE[pieceType][square] : MG_TABLE[pieceType][square];
    }
};

#endif //SENTINEL_PST_H
