#ifndef SENTINEL_PST_H
#define SENTINEL_PST_H

#include <cassert>

// From texel tuner.
// !! NOT USED ANYMORE !!

constexpr int32_t S(const int32_t mg, const int32_t eg)
{
    return static_cast<int32_t>(static_cast<uint32_t>(eg) << 16) + mg;
}

static constexpr int32_t mg_score(int32_t score)
{
    return static_cast<int16_t>(score);
}

static constexpr int32_t eg_score(int32_t score)
{
    return static_cast<int16_t>((score + 0x8000) >> 16);
}
// End from texel tuner.

struct PST{
    static inline constexpr int PIECE_EVAL[] = {S(44,71), S(165,145), S(155,142), S(198,237), S(412,425), S(0,0)};
    static inline constexpr int PAWN_TABLE[] = {S(0,0), S(0,0), S(0,0), S(0,0), S(0,0), S(0,0), S(0,0), S(0,0), S(48,308), S(64,290), S(55,281), S(56,242), S(76,228), S(41,229), S(-20,285), S(-57,320), S(3,145), S(18,149), S(59,118), S(65,93), S(72,80), S(94,77), S(61,114), S(19,121), S(-2,67), S(17,54), S(21,41), S(38,21), S(56,18), S(45,27), S(35,39), S(12,44), S(-16,40), S(-1,34), S(20,18), S(35,10), S(35,13), S(34,13), S(13,21), S(-8,23), S(-12,31), S(-6,27), S(11,19), S(15,23), S(30,21), S(26,17), S(31,12), S(8,15), S(-13,40), S(7,29), S(0,34), S(2,37), S(12,37), S(28,28), S(42,15), S(-2,20), S(0,0), S(0,0), S(0,0), S(0,0), S(0,0), S(0,0), S(0,0), S(0,0)};
    static inline constexpr int KNIGHT_TABLE[] = {S(-42,25), S(-137,85), S(-113,66), S(-63,58), S(42,42), S(-149,44), S(-67,59), S(8,0), S(29,67), S(60,59), S(81,57), S(-47,94), S(5,65), S(79,31), S(42,35), S(62,55), S(60,47), S(70,56), S(77,105), S(123,92), S(145,72), S(66,89), S(100,33), S(103,24), S(79,52), S(66,75), S(99,105), S(108,112), S(97,111), S(139,95), S(70,78), S(129,42), S(72,50), S(38,74), S(85,107), S(80,109), S(87,112), S(94,99), S(73,64), S(78,49), S(57,36), S(41,63), S(78,82), S(82,99), S(95,96), S(90,75), S(69,54), S(63,42), S(49,53), S(48,43), S(34,57), S(57,60), S(56,59), S(44,57), S(63,46), S(77,71), S(38,57), S(79,48), S(27,45), S(49,48), S(58,43), S(58,36), S(79,54), S(65,58)};
    static inline constexpr int BISHOP_TABLE[] = {S(42,83), S(-95,121), S(-124,120), S(-103,117), S(-44,106), S(-92,106), S(-50,104), S(53,74), S(61,90), S(92,91), S(65,100), S(-45,116), S(61,88), S(92,72), S(73,84), S(65,80), S(26,118), S(100,96), S(87,104), S(106,92), S(103,87), S(98,102), S(109,83), S(91,99), S(80,97), S(86,105), S(104,98), S(121,111), S(116,106), S(95,99), S(104,88), S(83,89), S(94,83), S(83,99), S(96,106), S(113,110), S(108,104), S(98,100), S(89,88), S(103,79), S(110,88), S(109,95), S(106,100), S(105,100), S(102,102), S(110,95), S(105,89), S(116,88), S(121,84), S(123,78), S(114,77), S(93,90), S(103,90), S(102,87), S(138,86), S(113,73), S(101,82), S(120,87), S(118,74), S(75,93), S(83,92), S(96,89), S(104,79), S(97,71)};
    static inline constexpr int ROOK_TABLE[] = {S(95,220), S(84,222), S(48,230), S(60,216), S(78,207), S(48,211), S(106,200), S(118,206), S(103,220), S(96,227), S(131,217), S(157,201), S(145,191), S(168,173), S(126,190), S(159,189), S(85,215), S(111,207), S(105,205), S(125,191), S(148,168), S(167,168), S(191,160), S(162,171), S(79,209), S(100,202), S(103,204), S(110,193), S(116,181), S(148,167), S(150,167), S(146,172), S(88,191), S(73,198), S(87,197), S(99,190), S(104,180), S(115,178), S(130,166), S(121,170), S(77,188), S(89,181), S(89,185), S(102,179), S(111,173), S(123,167), S(157,146), S(126,154), S(82,177), S(94,176), S(97,184), S(104,180), S(112,172), S(115,171), S(143,152), S(79,174), S(117,185), S(111,189), S(116,194), S(123,188), S(126,180), S(120,182), S(124,182), S(133,161)};
    static inline constexpr int QUEEN_TABLE[] = {S(294,333), S(211,413), S(205,428), S(217,431), S(259,418), S(206,432), S(334,331), S(357,326), S(299,355), S(286,359), S(263,416), S(182,483), S(236,467), S(300,406), S(329,341), S(358,357), S(289,352), S(284,364), S(282,389), S(298,392), S(315,396), S(332,401), S(380,331), S(388,341), S(282,350), S(296,355), S(291,368), S(286,397), S(295,410), S(304,394), S(328,373), S(331,376), S(307,330), S(283,369), S(302,356), S(297,390), S(304,375), S(312,373), S(313,372), S(330,355), S(306,318), S(326,310), S(306,354), S(316,332), S(313,345), S(327,338), S(338,327), S(326,330), S(308,322), S(321,309), S(332,297), S(334,306), S(333,305), S(328,291), S(335,278), S(345,273), S(314,310), S(308,307), S(322,295), S(341,287), S(323,286), S(283,307), S(300,307), S(335,264)};
    static inline constexpr int KING_TABLE[] = {S(59,-112), S(43,-46), S(65,-46), S(10,-23), S(46,-28), S(22,-16), S(48,-21), S(57,-83), S(-43,-19), S(34,24), S(24,30), S(30,23), S(35,37), S(12,47), S(19,46), S(-31,1), S(-28,-3), S(47,39), S(6,63), S(17,68), S(-1,73), S(80,70), S(31,60), S(-5,6), S(-17,-18), S(-22,41), S(-52,73), S(-99,102), S(-105,103), S(-68,83), S(-50,56), S(-128,17), S(-46,-26), S(-27,23), S(-72,65), S(-116,95), S(-106,96), S(-83,68), S(-73,36), S(-150,8), S(-34,-30), S(-1,9), S(-61,47), S(-82,66), S(-66,64), S(-59,50), S(-21,18), S(-55,-10), S(32,-47), S(11,-5), S(-15,16), S(-61,33), S(-55,36), S(-26,22), S(30,-4), S(19,-35), S(1,-76), S(59,-58), S(40,-33), S(-72,-6), S(-5,-28), S(-49,-13), S(46,-47), S(22,-82)};

    const static inline int32_t * PST_TABLE[6] =
    {
            PAWN_TABLE,
            KNIGHT_TABLE,
            BISHOP_TABLE,
            ROOK_TABLE,
            QUEEN_TABLE,
            KING_TABLE
    };

    #define FLIP(sq) ((sq)^56)

    static inline int TABLE[12][64];

    static void init() {
        // +6 for black side.
        for (int j = 0; j < 6; j++) {
            for (int square = 0; square < 64; square++) {
                // white side.
                TABLE[j][square] = PIECE_EVAL[j] + PST_TABLE[j][square];
                TABLE[j + 6][square] = PIECE_EVAL[j] + PST_TABLE[j][FLIP(square)];
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
    template<bool white>
    static inline int getValue(int pieceType, int square){
        pieceType += white ? 0 : 6;
        return TABLE[pieceType][square];
    }
};

#endif //SENTINEL_PST_H
