#include "board.h"
#include "movegen.h"

static inline constexpr int MAX_KING_DISTANCE = 14;
static inline constexpr int KING_DISTANCE_MULTIPLIER = S(4,18);
static inline constexpr int TWO_BISHOPS_BONUS = S(27,65);
static inline constexpr int CASTLING_BONUS = S(10,11);
static inline constexpr int PASSED_PAWN_BONUS = S(8,31);
static inline constexpr int ISOLATED_PAWN_SUBTRACT = S(-13,-5);
static inline constexpr int STACKED_PAWN_SUBTRACT = S(0,-7);
static inline constexpr int FRIEND_PAWN_BONUS = S(13,13);
static inline constexpr int ROOK_SEMI_OPEN_FILE_BONUS = S(-14,6);
static inline constexpr int ROOK_OPEN_FILE_BONUS = S(44,-4);
static inline constexpr int BISHOP_MOBILITY_BONUS[] = {S(0,0), S(35,66), S(58,70), S(74,93), S(91,97), S(99,107), S(108,122), S(117,125), S(126,132), S(125,131), S(132,134), S(136,123), S(136,122), S(161,108)};
static inline constexpr int KNIGHT_MOBILITY_BONUS[] = {S(0,0), S(0,0), S(20,102), S(79,102), S(90,134), S(0,0), S(129,135), S(0,0), S(118,122)};
static inline constexpr int ROOK_MOBILITY_BONUS[] = {S(0,0), S(0,0), S(119,176), S(122,192), S(129,205), S(134,211), S(137,217), S(141,222), S(149,221), S(155,223), S(160,226), S(164,225), S(166,227), S(166,229), S(163,219)};
static inline constexpr int QUEEN_MOBILITY_BONUS[] = {S(0,0), S(0,0), S(0,0), S(211,65), S(293,175), S(342,216), S(365,186), S(360,269), S(360,292), S(359,312), S(358,332), S(356,353), S(357,366), S(359,374), S(362,383), S(364,388), S(364,397), S(364,404), S(365,408), S(366,412), S(367,416), S(370,418), S(368,420), S(374,420), S(377,419), S(400,413), S(418,400), S(401,417)};
static inline constexpr int KING_VIRTUAL_MOBILITY[] = {S(76,-26), S(76,-11), S(63,4), S(50,6), S(40,12), S(36,10), S(31,9), S(26,5), S(14,18), S(1,20), S(-9,20), S(-25,24), S(-35,25), S(-50,27), S(-65,27), S(-83,28), S(-93,23), S(-102,22), S(-110,17), S(-101,7), S(-99,0), S(-106,-6), S(-124,-14), S(-92,-27), S(-130,-36), S(-84,-53), S(-122,-67), S(-61,-77)};

static inline int gamePhaseInc[] = { 0, 1, 1, 2, 4, 0 };

inline int getManhattanDist(const int posA[2], const int posB[2]){
    return std::abs(posA[0] - posB[0]) + std::abs(posA[1] - posB[1]);
}

int Board::eval() {
    uint64_t all;
    uint64_t white = 0ULL;
    uint64_t black = 0ULL;
    for(int j = 0; j < 6; j++){
        white |= whitePieces[j];
        black |= blackPieces[j];
    }
    all = white | black;

    auto whiteScore = evalSide(whitePieces, true, all, white);
    auto blackScore = evalSide(blackPieces, false, all, black);

    // Eval some things for endgames.
    // Now only king distances.
    // only endgames

    auto whiteKingIndex = bit_ops::bitScanForward(whitePieces[KING]);
    auto blackKingIndex = bit_ops::bitScanForward(blackPieces[KING]);

    int whiteKingPos[2] = {whiteKingIndex/8, whiteKingIndex % 8};
    int blackKingPos[2] = {blackKingIndex/8, blackKingIndex % 8};

    // Manhattan distance between kings.
    auto distance = getManhattanDist(whiteKingPos, blackKingPos);
    int scoreToAdd = KING_DISTANCE_MULTIPLIER * (MAX_KING_DISTANCE - distance);

    // Add bonus for side, if king is closer to enemy king
    if(eg_score(whiteScore) > eg_score(blackScore)){
        whiteScore += scoreToAdd;
    }
    else{
        blackScore += scoreToAdd;
    }

    // add bonus, if castling is still possible
    // if king is attacked, dont move king, just try block.
    whiteScore += (castling[pieceColor::WHITE][0] || castling[pieceColor::WHITE][1]) ? CASTLING_BONUS : 0;
    blackScore += (castling[pieceColor::BLACK][0] || castling[pieceColor::BLACK][1]) ? CASTLING_BONUS : 0;

    int phase = countPhase(whitePieces) + countPhase(blackPieces);
    if (phase > 24) phase = 24;
    int egPhase = 24 - phase;

    int mgScore = (mg_score(whiteScore) - mg_score(blackScore)) * (whoPlay ? 1 : -1);
    int egScore = (eg_score(whiteScore) - eg_score(blackScore)) * (whoPlay ? 1 : -1);

    int taperedEval = ((mgScore*phase + egScore * egPhase) / 24);
    return taperedEval;
}

int Board::countPhase(uint64_t *bbs) const{
    int phase = 0;
    for(int j = 1; j < 5; j++){
        auto bb = bbs[j];
        phase += bit_ops::countBits(bb) * gamePhaseInc[j];
    }
    return phase;
}

int32_t Board::evalSide(uint64_t *bbs, bool white, const uint64_t& all, const uint64_t& us) const{
    int32_t eval = 0;
    evalPawns(bbs, white, eval);
    evalBishops(bbs, white, all, eval);
    evalKnights(bbs, white, eval);
    evalRooks(bbs, white, all, eval);
    evalQueens(bbs, white, all, eval);
    evalKing(bbs, white, all, us, eval);

    return eval;
}


void Board::evalKnights(uint64_t *bbs, bool white, int32_t & eval) const {

    auto bb = bbs[KNIGHT];
    while(bb){
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        eval += PST::getValue(white, KNIGHT, pos);

        auto attacks = Movegen::KNIGHT_MOVES[pos];
        auto attackCount = bit_ops::countBits(attacks);
        eval += KNIGHT_MOBILITY_BONUS[attackCount];
    }
}

void Board::evalQueens(uint64_t *bbs, bool white, const uint64_t& all, int32_t & eval) const {
    auto bb = bbs[QUEEN];
    while(bb){
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        eval += PST::getValue(white, QUEEN, pos);

        auto attacks = Magics::getRookMoves(all, pos);
        attacks |= Magics::getBishopMoves(all, pos);
        auto attackCount = bit_ops::countBits(attacks);
        eval += QUEEN_MOBILITY_BONUS[attackCount];
    }
}


void Board::evalRooks(uint64_t *bbs, bool white, const uint64_t& all, int32_t & eval) const {
    auto friendlyPawns = white ? whitePieces[PAWN] : blackPieces[PAWN];
    auto enemyPawns = white ? blackPieces[PAWN] : whitePieces[PAWN];
    auto allPawns = friendlyPawns | enemyPawns;

    auto bb = bbs[ROOK];
    while(bb){
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        eval += PST::getValue(white, ROOK, pos);

        auto attacks = Magics::getRookMoves(all, pos);
        auto attackCount = bit_ops::countBits(attacks);
        eval += ROOK_MOBILITY_BONUS[attackCount];

        auto column = pos % 8;
        // open files -> no pawns on it.
        if((allPawns & LINE_BITBOARDS[column]) == 0){
            eval += ROOK_OPEN_FILE_BONUS;
        }
        // semi-open files -> only enemy pawns on it.
        if(((LINE_BITBOARDS[column] & allPawns) ^ enemyPawns) == 0){
            eval += ROOK_SEMI_OPEN_FILE_BONUS;
        }
    }
}


void Board::evalKing(uint64_t *bbs, bool white, const uint64_t& all, const uint64_t& us, int32_t & eval) const {
    auto bb = bbs[KING];
    auto pos = bit_ops::bitScanForwardPopLsb(bb);
    eval += PST::getValue(white, KING, pos);

    // King virtual mobility - apply in middlegame only.
    auto attacks = Magics::getRookMoves(all, pos);
    attacks |= Magics::getBishopMoves(all, pos);
    attacks &= ~us;
    auto attackCount = bit_ops::countBits(attacks);
    eval += KING_VIRTUAL_MOBILITY[attackCount];
}


void Board::evalBishops(uint64_t *bbs, bool white, const uint64_t& all, int32_t & eval) const {
    // count number of bishops, add bonus.
    int bishopCount = 0;
    auto bb = bbs[BISHOP];
    while(bb){
        bishopCount++;
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        eval += PST::getValue(white, BISHOP, pos);

        auto attacks = Magics::getBishopMoves(all, pos);
        auto attackCount = bit_ops::countBits(attacks);
        eval += BISHOP_MOBILITY_BONUS[attackCount];
    }
    eval += bishopCount >= 2 ? TWO_BISHOPS_BONUS : 0; // Rays goes brr.
}

void Board::evalPawns(uint64_t *bbs, bool white, int32_t & eval) const{
    // Pawn eval.
    auto enemyPawnsBB = white ? blackPieces[PAWN] : whitePieces[PAWN];
    auto friendlyPawnsBB = white ? whitePieces[PAWN] : blackPieces[PAWN];
    auto bb = bbs[PAWN];

    while(bb){
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        eval += PST::getValue(white, PAWN, pos);

        int distanceFromPromotionRev = white ? ((7 - (pos / 8)) - 1) : ((7 - (7 - (pos / 8))) - 1);
        assert(distanceFromPromotionRev >= 0);
        if((enemyPawnsBB & PAWN_PASSED_BITBOARDS[!white][pos]) == 0ULL){
            eval += PASSED_PAWN_BONUS * distanceFromPromotionRev;
        }

        // Add bonus, if pawns are in some sort of structure.
        if((bb & PAWN_FRIENDS_BITBOARDS[pos]) != 0){
            eval += FRIEND_PAWN_BONUS;
        }

        // If pawn is isolated, subtract value from current eval.
        auto column = pos % 8;
        if((friendlyPawnsBB & PAWN_ISOLATION_BITBOARDS[column]) == 0){
            eval += ISOLATED_PAWN_SUBTRACT;
        }

        // doubled/tripled pawns.
        uint64_t tmp = 0ULL;
        bit_ops::setNthBit(tmp, pos);
        if((friendlyPawnsBB & (LINE_BITBOARDS[column] ^ tmp)) != 0){
            eval += STACKED_PAWN_SUBTRACT;
        }
    }
}

void Board::initPawnEvalBBS(){
    initPassedPawnBBS();
    initPawnIsolationBBS();
    initPawnFriendsBBS();
    initLineBBS();
}

void Board::initPassedPawnBBS(){
    for(int j = 0; j < 64; j++){
        PAWN_PASSED_BITBOARDS[WHITE][j] = 0ULL;
        PAWN_PASSED_BITBOARDS[BLACK][j] = 0ULL;
    }

    for(int square = 16; square <= 55; square++){
        int tmp = square - 8;
        while(tmp >= 8){
            setPassedPawnBits(square, tmp, WHITE);
            tmp -= 8;
        }
    }

    for(int square = 8; square <= 47; square++){
        int tmp = square + 8;
        while(tmp <= 55){
            setPassedPawnBits(square, tmp, BLACK);
            tmp += 8;
        }
    }
}

void Board::setPassedPawnBits(int square, int tmp, int index) {
    bit_ops::setNthBit(PAWN_PASSED_BITBOARDS[index][square], tmp);
    if(square % 8 == 0 || (square + 1)% 8 != 0) bit_ops::setNthBit(PAWN_PASSED_BITBOARDS[index][square], tmp + 1);
    if((square + 1) % 8 == 0 || (square % 8) != 0) bit_ops::setNthBit(PAWN_PASSED_BITBOARDS[index][square], tmp - 1);
}

void Board::initPawnFriendsBBS(){
    for(int j = 0; j < 64; j++){
        PAWN_FRIENDS_BITBOARDS[j] = 0ULL;
    }
    // distance 1 radius mask.
    for(int square = 8; square <= 55; square++){
        setFriendRadiusBits(square);
    }
}

void Board::setFriendRadiusBits(int square){
    auto onLeftEdge = square % 8 == 0;
    auto onRightEdge = (square + 1) % 8 == 0;

    if(!onLeftEdge){
        bit_ops::setNthBit(PAWN_FRIENDS_BITBOARDS[square], square - 1);
        bit_ops::setNthBit(PAWN_FRIENDS_BITBOARDS[square], square + 7);
        bit_ops::setNthBit(PAWN_FRIENDS_BITBOARDS[square], square - 9);
    }

    if(!onRightEdge){
        bit_ops::setNthBit(PAWN_FRIENDS_BITBOARDS[square], square + 1);
        bit_ops::setNthBit(PAWN_FRIENDS_BITBOARDS[square], square - 7);
        bit_ops::setNthBit(PAWN_FRIENDS_BITBOARDS[square], square + 9);
    }
}

void Board::initLineBBS(){
    uint64_t starting = 0x101010101010101;
    for(int column = 0; column <= 7; column++) {
        LINE_BITBOARDS[column] = 0ULL;
        LINE_BITBOARDS[column] |= starting << (column);
    }
}

void Board::initPawnIsolationBBS() {
    uint64_t starting = 0x101010101010101;

    // simple line init for doubled and tripled pawns.
    for(int column = 0; column <= 7; column++){
        PAWN_ISOLATION_BITBOARDS[column] = 0ULL;
    }


    // not on the edge.
    for(int column = 1; column <= 6; column++){
        PAWN_ISOLATION_BITBOARDS[column] |= starting << (column - 1);
        PAWN_ISOLATION_BITBOARDS[column] |= starting << (column + 1);
    }
    // left edge
    PAWN_ISOLATION_BITBOARDS[0] |= starting << 1;

    // right edge.
    PAWN_ISOLATION_BITBOARDS[7] |= starting << 6;

}
