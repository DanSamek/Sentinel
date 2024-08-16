#ifndef SENTINEL_PST_H
#define SENTINEL_PST_H

#include <cassert>

// From texel tuner.

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
    static constexpr int32_t PIECE_EVAL[6] = {S(100,115), S(305,290), S(325,340), S(500,550), S(900,1000), S(0,0)};

    static inline constexpr int PAWN_TABLE[] = {S(0,0), S(0,0), S(0,0), S(0,0), S(0,0), S(0,0), S(0,0), S(0,0), S(105,488), S(127,466), S(115,453), S(115,404), S(140,385), S(100,387), S(27,460), S(-21,505), S(57,273), S(74,278), S(122,239), S(130,207), S(138,190), S(165,185), S(125,233), S(75,242), S(48,172), S(72,155), S(78,137), S(97,113), S(118,108), S(105,119), S(94,135), S(66,142), S(32,136), S(50,127), S(76,107), S(94,97), S(94,101), S(92,101), S(67,111), S(41,114), S(36,123), S(44,119), S(65,109), S(70,114), S(87,111), S(82,106), S(89,99), S(61,104), S(36,135), S(60,122), S(52,127), S(54,131), S(67,132), S(85,119), S(102,104), S(48,110), S(0,0), S(0,0), S(0,0), S(0,0), S(0,0), S(0,0), S(0,0), S(0,0)};
    static inline constexpr int KNIGHT_TABLE[] = {S(0,76), S(-77,168), S(-65,136), S(-7,125), S(120,104), S(-107,107), S(4,135), S(61,43), S(122,144), S(142,126), S(167,128), S(13,176), S(76,139), S(164,95), S(119,96), S(161,128), S(141,111), S(154,128), S(179,205), S(233,189), S(260,161), S(166,183), S(189,98), S(191,82), S(163,117), S(150,152), S(205,206), S(216,214), S(204,212), S(253,192), S(154,155), S(223,104), S(155,115), S(116,151), S(189,208), S(183,210), S(192,215), S(200,197), S(157,138), S(162,112), S(137,96), S(119,136), S(181,176), S(185,198), S(200,194), S(195,166), S(153,124), S(144,104), S(144,126), S(126,106), S(111,128), S(138,132), S(137,131), S(122,130), S(144,110), S(178,149), S(95,117), S(180,119), S(102,108), S(128,112), S(139,105), S(138,97), S(181,127), S(128,118)};
    static inline constexpr int BISHOP_TABLE[] = {S(130,167), S(-36,219), S(-69,217), S(-44,213), S(24,198), S(-31,198), S(19,196), S(144,156), S(152,177), S(188,179), S(156,191), S(24,210), S(152,174), S(188,154), S(166,171), S(156,164), S(110,214), S(197,185), S(182,195), S(205,180), S(202,173), S(195,192), S(208,167), S(188,189), S(174,187), S(181,196), S(202,187), S(223,204), S(217,198), S(192,189), S(203,175), S(177,175), S(191,168), S(177,189), S(193,198), S(214,203), S(207,196), S(195,191), S(184,175), S(201,163), S(210,175), S(208,183), S(204,191), S(203,190), S(200,194), S(209,184), S(204,176), S(216,174), S(223,170), S(224,162), S(214,160), S(189,178), S(200,177), S(200,173), S(242,171), S(212,155), S(199,167), S(221,172), S(219,156), S(167,181), S(178,180), S(192,177), S(202,163), S(194,152)};
    static inline constexpr int ROOK_TABLE[] = {S(209,401), S(197,403), S(153,415), S(169,396), S(189,383), S(153,389), S(223,375), S(237,382), S(219,400), S(211,410), S(252,397), S(283,377), S(270,363), S(297,340), S(246,362), S(285,361), S(198,394), S(229,384), S(221,381), S(246,364), S(273,334), S(295,333), S(324,323), S(289,336), S(190,387), S(216,378), S(219,381), S(228,365), S(234,351), S(272,332), S(274,332), S(270,339), S(200,363), S(184,373), S(200,371), S(213,362), S(220,348), S(233,346), S(251,331), S(239,335), S(187,359), S(202,350), S(202,355), S(217,347), S(229,339), S(242,332), S(283,305), S(246,314), S(194,344), S(208,343), S(212,354), S(220,349), S(229,338), S(233,336), S(267,312), S(190,340), S(236,357), S(229,360), S(235,368), S(242,359), S(247,349), S(239,351), S(243,352), S(255,327)};
    static inline constexpr int QUEEN_TABLE[] = {S(572,650), S(466,761), S(461,777), S(484,774), S(526,764), S(451,792), S(620,646), S(647,639), S(580,676), S(565,682), S(538,755), S(441,844), S(506,822), S(581,743), S(617,656), S(650,677), S(568,672), S(563,688), S(560,720), S(579,725), S(599,729), S(620,734), S(676,641), S(685,655), S(560,669), S(577,676), S(572,693), S(565,731), S(577,748), S(587,726), S(615,698), S(619,702), S(589,643), S(562,694), S(584,678), S(578,724), S(587,703), S(597,700), S(597,698), S(618,675), S(587,626), S(611,618), S(589,674), S(601,647), S(597,663), S(614,653), S(626,639), S(612,643), S(590,633), S(606,614), S(619,600), S(621,611), S(620,609), S(614,591), S(622,577), S(635,569), S(597,617), S(590,611), S(606,596), S(628,584), S(607,584), S(560,613), S(582,613), S(624,556)};
    static inline constexpr int KING_TABLE[] = {S(73,-147), S(49,-62), S(75,-61), S(14,-32), S(55,-40), S(23,-23), S(51,-29), S(59,-107), S(-54,-26), S(39,30), S(29,39), S(35,29), S(41,48), S(12,61), S(22,60), S(-40,1), S(-34,-4), S(56,50), S(7,81), S(21,88), S(-2,95), S(97,91), S(35,78), S(-8,8), S(-22,-24), S(-27,53), S(-64,94), S(-121,133), S(-126,133), S(-82,107), S(-60,72), S(-153,21), S(-56,-34), S(-33,29), S(-87,84), S(-140,124), S(-127,125), S(-99,89), S(-86,46), S(-178,9), S(-41,-39), S(-1,11), S(-72,61), S(-97,85), S(-78,83), S(-69,65), S(-25,22), S(-66,-14), S(37,-62), S(13,-7), S(-17,20), S(-73,42), S(-66,47), S(-31,29), S(36,-6), S(22,-47), S(0,-99), S(69,-76), S(47,-44), S(-88,-8), S(1,-37), S(-59,-17), S(54,-62), S(26,-107)};
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
    static inline int getValue(bool color, int pieceType, int square){
        pieceType += color ? 0 : 6;
        return TABLE[pieceType][square];
    }
};

#endif //SENTINEL_PST_H
