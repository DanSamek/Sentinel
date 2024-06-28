#ifndef SENTINEL_PST_H
#define SENTINEL_PST_H

#include <cassert>

struct PST{
    static constexpr int PIECE_EVAL_MG[6] = {100, 305, 325, 512, 942, 10000};
    static constexpr int PIECE_EVAL_EG[6] = {112, 290, 320, 564, 1048, 10000};

    // Handcrafted PST's - inspired from chessboard (my brain) + pst avg values to have normal values (chessprogramming).
    // All white perspective (from bottom) we do a bit flip for getting black right value.

    static constexpr inline int MG_PAWN_TABLE[64] = {
            0,     0,   0,   0,   0,   0,   0,   0,
            -34, -24, -20, -15, -15, -20, -24, -34,
            -22, -18,  -8, -12, -12,  -8, -18, -22,
            -16, -12,  4,   7,  7, 4, -12, -16,
            -12,  -8,  5,  20,  20,  5,  -8, -12,
            -6,   -4,  4,  15,  15,  4,  -4,  -6,
            10,   15,  10,  5,  5,  10,  15,  10,
            0,     0,   0,   0,   0,   0,   0,   0,
    };

    static constexpr inline int EG_PAWN_TABLE[64] = {
            0,    0,    0,    0,    0,    0,    0,    0,
            60,  60,   60,   60,    60,  60,   60,   60,
            40,  40,   40,   40,    40,  40,   40,   40,
            25,  25,   25,   25,    25,  25,   25,   25,
            10,  10,   10,   10,    10,  10,   10,   10,
            5,    5,    5,    5,    5,    5,    5,    5,
            0,    0,    0,    0,    0,    0,    0,    0
    };

    static constexpr inline int MG_KNIGHT_TABLE[64] = {
            -100, -30, -30, -30, -30, -30, -30, -100,
            -42,  -30,  -7, -15, -15,  -7, -30,  -42,
            -30,  -30, -12,  -5,  -5, -12, -30,  -30,
            -17,   13,  12,  13,  13,  12,  13,  -17,
            -15,   17,  18,  22,  22,  18,  17,  -15,
            -35,   15,  17,  16,  16,  17,  15,  -35,
            -75,  -35,   3,   4,   4,   3,  -35  -75,
            -100, -35, -35, -35, -35, -35,  -35,-100
    };

    static constexpr inline int EG_KNIGHT_TABLE[64] = {
            -50, -15, -15, -15, -15, -15, -15, -50,
            -22, -15,  -3,  -3,  -3,  -3, -15, -22,
            -15,  15,   12,   8,   8,   12,  15, -15,
            -17,  17,  25,  27,  27,  25,  17, -15,
            -15,  21,  30,  22,  22,  30,  21, -15,
            -15,   8,   7,  13,  13,   7,   8, -15,
            -22, -35, -10, -10, -10, -10, -35, -22,
            -50, -15, -15, -15, -15, -15, -15, -50
    };

    static constexpr inline int MG_BISHOP_TABLE[64] = {
            -40,  -40,  -40,  -40,  -40,  -40,  -40,  -40,
            -30,  -30,  -30,  -30,  -30,  -30,  -30,  -30,
            -20,  -20,  -20,  -20,  -20,  -20,  -20,  -20,
            -16,   -7,    5,    2,    2,    5,   -7,  -16,
            -8,   21,   23,   14,   14,   23,   21,   -8,
            -4,   17,   21,   15,   15,   21,   17,   -4,
            -40,   21,   -3,  -17,  -17,   -3,   21,  -40,
            -50,  -30,  -17,  -25,  -25,  -17,  -30,  -50
    };

    static constexpr inline int EG_BISHOP_TABLE[64] = {
            -30,  -20,  -13,   -7,   -7,  -13,  -20,  -30,
            -20,  -15,    0,    2,    2,    0,  -15,  -20,
            -17,   -7,    2,    7,    7,    2,   -7,  -17,
            -7,    3,   14,   23,   23,   14,    3,   -7,
            -7,    3,    7,   17,   17,    7,    3,   -7,
            -17,   -5,    3,    7,    7,    3,   -5,  -17,
            -20,  -20,   -5,   -3,   -3,   -5,  -20,  -20,
            -30,  -20,  -20,  -20,  -20,  -20,  -20,  -30
    };


    static constexpr inline int MG_ROOK_TABLE[64] = {
            -40,  -40,  -40,  -40,  -40,  -40,  -40,  -40,
            -30,  -30,  -30,  -30,  -30,  -30,  -30,  -30,
            -20,  -20,  -20,  -20,  -20,  -20,  -20,  -20,
            -10,  -10,  -10,  -10,  -10,  -10,  -10,  -10,
            -5,   -5,   -5,   -5,   -5,   -5,   -5,   -5,
             2,    4,    8,    2,    2,    8,    4,    2,
             0,    3,    12,   20,   20,    12,    3,    0,
            -8,   -3,   8,   35,   35,   8,   -3,   -8
    };

    static constexpr inline int EG_ROOK_TABLE[64] = {
            -18,  -12,   -8,   -4,   -4,   -8,  -12,  -18,
            -12,   -6,    7,   12,   12,    7,   -6,  -12,
            -8,   15,   16,    8,    8,   16,   15,   -8,
            -4,   17,   13,    3,    3,   13,   17,   -4,
            0,   -2,    7,    7,    7,    7,   -2,    0,
            12,    7,    5,    13,    13,    5,    7,   12,
            18,   19,   13,    10,    10,   13,   19,   18,
            15,   13,   16,   15,   15,   16,   13,   15
    };

    static constexpr inline int MG_QUEEN_TABLE[64] = {
            -40,  -40,  -40,  -40,  -40,  -40,  -40,  -40,
            -30,  -30,  -30,  -30,  -30,  -30,  -30,  -30,
            -20,  -20,  -20,  -20,  -20,  -20,  -20,  -20,
            -10,  -10,   -7,   -5,   -5,   -7,  -10,  -10,
            -5,    3,    4,    5,    5,    4,    3,   -5,
            7,  21,   17,   15,   15,   17,   21,    7,
            2,    6,   25,   27,   27,   25,    6,    2,
            5,    9,   18,   33,   33,   18,    9,    5
    };

    static constexpr inline int EG_QUEEN_TABLE[64] = {
            7,  12,  7,   13,   13,  7,  12,  7,
            -10,   10,   5,   25,   25,   5,   10,  -10,
            -5,    6,   12,   35,   35,   25,    6,   -5,
            -3,   22,   32,   40,   40,   32,   22,   -3,
            -10,   13,   31,   40,   40,   31,   13,  -10,
            -20,   17,   17,   35,   35,   35,   17,  -20,
            -30,    7,   13,   25,   25,   13,    7,  -30,
            -45,  -30,   20,   10,   10,   20,  -30,  -45
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
            -32,  -18,  -10,   -3,   -3,  -10,  -18,  -32,
            -22,    6,    7,   12,   12,    7,    6,  -22,
            -7,    8,   12,   23,   23,   12,    8,   -7,
            -5,    5,   20,   35,   35,   20,    5,   -5,
            -7,    3,   20,   35,   35,   20,    3,   -7,
            -30,   -3,    3,   12,   12,    3,   -3,  -30,
            -40,  -30,   -7,   -5,   -5,   -7,  -30,  -40,
            -40,  -40,  -30,   -10,   -10,  -30,  -40,  -40

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

    static void init(){
        // +6 for black side.
        for(int j = 0; j < 6; j++){
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
