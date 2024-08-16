#include "board.h"
#include "movegen.h"

static inline constexpr int MAX_KING_DISTANCE = 14;
static inline constexpr int KING_DISTANCE_MULTIPLIER = S(4,15);
static inline constexpr int TWO_BISHOPS_BONUS = S(32,84);
static inline constexpr int CASTLING_BONUS = S(6,0);
static inline constexpr int PASSED_PAWN_BONUS = S(9,40);
static inline constexpr int ISOLATED_PAWN_SUBTRACT = S(-16,-5);
static inline constexpr int STACKED_PAWN_SUBTRACT = S(0,-10);
static inline constexpr int FRIEND_PAWN_BONUS = S(16,17);
static inline constexpr int ROOK_SEMI_OPEN_FILE_BONUS = S(-16,6);
static inline constexpr int ROOK_OPEN_FILE_BONUS = S(52,-7);
static inline constexpr int BISHOP_MOBILITY_BONUS[] = {S(0,0), S(143,184), S(172,186), S(191,216), S(211,222), S(221,235), S(231,253), S(242,257), S(252,266), S(251,264), S(259,267), S(264,253), S(264,252), S(296,233)};
static inline constexpr int KNIGHT_MOBILITY_BONUS[] = {S(0,0), S(0,0), S(168,255), S(202,240), S(233,289), S(0,0), S(278,285), S(0,0), S(247,253)};
static inline constexpr int ROOK_MOBILITY_BONUS[] = {S(0,0), S(0,0), S(277,376), S(282,397), S(290,416), S(296,423), S(299,430), S(304,436), S(313,434), S(320,436), S(326,440), S(331,438), S(333,440), S(333,443), S(330,430)};
static inline constexpr int QUEEN_MOBILITY_BONUS[] = {S(0,0), S(0,0), S(0,0), S(400,128), S(626,400), S(663,521), S(690,496), S(686,601), S(687,629), S(687,657), S(686,682), S(684,710), S(687,728), S(689,738), S(692,750), S(695,756), S(695,768), S(696,777), S(696,783), S(697,788), S(699,793), S(702,796), S(698,799), S(705,801), S(708,798), S(734,793), S(763,767), S(740,793)};
static inline constexpr int KING_VIRTUAL_MOBILITY[] = {S(94,-37), S(93,-19), S(78,3), S(64,4), S(51,13), S(47,11), S(42,10), S(35,5), S(21,22), S(5,26), S(-6,26), S(-25,31), S(-38,33), S(-55,36), S(-73,35), S(-95,38), S(-107,31), S(-118,30), S(-127,24), S(-116,11), S(-113,1), S(-122,-5), S(-142,-15), S(-103,-32), S(-150,-42), S(-92,-63), S(-141,-80), S(-69,-91)};

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
    blackScore += (castling[pieceColor::WHITE][0] || castling[pieceColor::WHITE][1]) ? CASTLING_BONUS : 0;

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
            eval -= ISOLATED_PAWN_SUBTRACT;
        }

        // doubled/tripled pawns.
        uint64_t tmp = 0ULL;
        bit_ops::setNthBit(tmp, pos);
        if((friendlyPawnsBB & (LINE_BITBOARDS[column] ^ tmp)) != 0){
            eval -= STACKED_PAWN_SUBTRACT;
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
