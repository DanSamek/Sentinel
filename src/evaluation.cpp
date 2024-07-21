#include <board.h>
#include <movegen.h>

static inline constexpr int MAX_KING_DISTANCE = 14;
static inline constexpr int KING_DISTANCE_MULTIPLIER = 10; // TODO buff!!
static inline constexpr int TWO_BISHOPS_BONUS = 30;
static inline constexpr int CASTLING_BONUS = 12;
static inline constexpr int PASSED_PAWN_BONUS = 15;
static inline constexpr int ISOLATED_PAWN_SUBTRACT = 15;
static inline constexpr int STACKED_PAWN_SUBTRACT = 4;
static inline constexpr int FRIEND_PAWN_BONUS = 8;

static inline constexpr int ROOK_SEMI_OPEN_FILE_BONUS = 10;
static inline constexpr int ROOK_OPEN_FILE_BONUS = 20;


// pseudo-legal mobility bonus [totalSquares possible] -> value.
// 0-13
static inline constexpr int BISHOP_MOBILITY_BONUS[14] = {
    -23,
    -17,
    -14,
    -8,
    -6,
    -2,
    2,
    6,
    9,
    12,
    15,
    16,
    15,
    14
};

// 8 + 1 [0 moves]    [0,1] not possible thanks to pseudo legal movegen.
static inline constexpr int KNIGHT_MOBILITY_BONUS[9] = {
    -100,
    -100,
    -5,
    -3,
    0,
    6,
    8,
    10,
    12,
};

// 14 + 1 [0]
static inline constexpr int ROOK_MOBILITY_BONUS[15] = {
    -31,
    -26,
    -21,
    -17,
    -12,
    -5,
    0,
    4,
    6,
    10,
    14,
    18,
    20,
    19,
    18
};

// 27
// Low values for a queen.
static inline constexpr int QUEEN_MOBILITY_BONUS[28] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    13,
    14,
    14,
    14,
    15,
    15,
    14,
    14,
    14,
    13,
    13,
    13,
    13,
    13
};

// 27
// Handling king as a queen -> add penalization if king has a lot of mobility.
static inline constexpr int KING_VIRTUAL_MOBILITY[28] = {
    0,
    9,
    12,
    14,
    13,
    12,
    11,
    10,
    8,
    6,
    4,
    2,
    -2,
    -4,
    -6,
    -10,
    -11,
    -12,
    -13,
    -14,
    -16,
    -17,
    -19,
    -20,
    -22,
    -24,
    -26,
    -30
};


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

    auto [whiteScoreMg, whiteScoreEg] = evalSide(whitePieces, true, all, white);
    auto [blackScoreMg, blackScoreEg] = evalSide(blackPieces, false, all, black);

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
    if(whiteScoreEg > blackScoreEg){
        whiteScoreEg += scoreToAdd;
    }
    else{
        blackScoreEg += scoreToAdd;
    }

    // add bonus, if castling is still possible
    // if king is attacked, dont move king, just try block.
    whiteScoreMg += (castling[pieceColor::WHITE][0] || castling[pieceColor::WHITE][1]) ? CASTLING_BONUS : 0;
    blackScoreMg += (castling[pieceColor::WHITE][0] || castling[pieceColor::WHITE][1]) ? CASTLING_BONUS : 0;

    int phase = countPhase(whitePieces) + countPhase(blackPieces);
    if (phase > 24) phase = 24;
    int egPhase = 24 - phase;

    int mgScore = (whiteScoreMg - blackScoreMg) * (whoPlay ? 1 : -1);
    int egScore = (whiteScoreEg - blackScoreEg) * (whoPlay ? 1 : -1);

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

std::pair<int, int> Board::evalSide(uint64_t *bbs, bool white, const uint64_t& all, const uint64_t& us) const{
    int evalMg = 0;
    int evalEg = 0;

    evalPawns(bbs, white, evalMg, evalEg);
    evalBishops(bbs, white, all, evalMg, evalEg);
    evalKnights(bbs, white, evalMg, evalEg);
    evalRooks(bbs, white, all, evalMg, evalEg);
    evalQueens(bbs, white, all, evalMg, evalEg);
    evalKing(bbs, white, all, us, evalMg, evalEg);

    return {evalMg, evalEg};
}


void Board::evalKnights(uint64_t *bbs, bool white, int& evalMg, int& evalEg) const {

    auto bb = bbs[KNIGHT];
    while(bb){
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        evalMg +=PST::getValue(white, KNIGHT, pos, false);
        evalEg += PST::getValue(white, KNIGHT, pos, true);

        auto attacks = Movegen::KNIGHT_MOVES[pos];
        auto attackCount = bit_ops::countBits(attacks);
        evalMg +=KNIGHT_MOBILITY_BONUS[attackCount];
        evalEg += KNIGHT_MOBILITY_BONUS[attackCount];
    }
}

void Board::evalQueens(uint64_t *bbs, bool white, const uint64_t& all, int& evalMg, int& evalEg) const {
    auto bb = bbs[QUEEN];
    while(bb){
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        evalMg += PST::getValue(white, QUEEN, pos, false);
        evalEg += PST::getValue(white, QUEEN, pos, true);

        auto attacks = Magics::getRookMoves(all, pos);
        attacks |= Magics::getBishopMoves(all, pos);
        auto attackCount = bit_ops::countBits(attacks);
        evalMg += QUEEN_MOBILITY_BONUS[attackCount];
        evalEg += QUEEN_MOBILITY_BONUS[attackCount];
    }
}


void Board::evalRooks(uint64_t *bbs, bool white, const uint64_t& all, int& evalMg, int& evalEg) const {
    auto friendlyPawns = white ? whitePieces[PAWN] : blackPieces[PAWN];
    auto enemyPawns = white ? blackPieces[PAWN] : whitePieces[PAWN];
    auto allPawns = friendlyPawns | enemyPawns;

    auto bb = bbs[ROOK];
    while(bb){
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        evalMg += PST::getValue(white, ROOK, pos, false);
        evalEg += PST::getValue(white, ROOK, pos, true);

        auto attacks = Magics::getRookMoves(all, pos);
        auto attackCount = bit_ops::countBits(attacks);
        evalMg += ROOK_MOBILITY_BONUS[attackCount];
        evalEg += ROOK_MOBILITY_BONUS[attackCount];

        auto column = pos % 8;
        // open files -> no pawns on it.
        if((allPawns & LINE_BITBOARDS[column]) == 0){
            evalMg += ROOK_OPEN_FILE_BONUS;
            evalEg += ROOK_OPEN_FILE_BONUS;
        }
        // semi-open files -> only enemy pawns on it.
        if(((LINE_BITBOARDS[column] & allPawns) ^ enemyPawns) == 0){
            evalMg += ROOK_SEMI_OPEN_FILE_BONUS;
            evalEg += ROOK_SEMI_OPEN_FILE_BONUS;
        }
    }
}


void Board::evalKing(uint64_t *bbs, bool white, const uint64_t& all, const uint64_t& us, int& evalMg, int& evalEg) const {
    auto bb = bbs[KING];
    auto pos = bit_ops::bitScanForwardPopLsb(bb);
    evalMg += PST::getValue(white, KING, pos, false);
    evalEg += PST::getValue(white, KING, pos, true);

    // King virtual mobility - apply in middlegame only.
    auto attacks = Magics::getRookMoves(all, pos);
    attacks |= Magics::getBishopMoves(all, pos);
    attacks &= ~us;
    auto attackCount = bit_ops::countBits(attacks);
    evalEg += KING_VIRTUAL_MOBILITY[attackCount];
}


void Board::evalBishops(uint64_t *bbs, bool white, const uint64_t& all, int& evalMg, int& evalEg) const {
    // count number of bishops, add bonus.
    int bishopCount = 0;
    auto bb = bbs[BISHOP];
    while(bb){
        bishopCount++;
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        evalMg +=PST::getValue(white, BISHOP, pos, false);
        evalEg +=PST::getValue(white, BISHOP, pos, true);

        auto attacks = Magics::getBishopMoves(all, pos);
        auto attackCount = bit_ops::countBits(attacks);
        evalMg += BISHOP_MOBILITY_BONUS[attackCount];
        evalEg += BISHOP_MOBILITY_BONUS[attackCount];
    }
    evalMg += bishopCount >= 2 ? TWO_BISHOPS_BONUS : 0; // Rays goes brr.
    evalEg += bishopCount >= 2 ? TWO_BISHOPS_BONUS : 0; // Rays goes brr.
}

void Board::evalPawns(uint64_t *bbs, bool white, int& evalMg, int& evalEg) const{
    // Pawn eval.
    auto enemyPawnsBB = white ? blackPieces[PAWN] : whitePieces[PAWN];
    auto friendlyPawnsBB = white ? whitePieces[PAWN] : blackPieces[PAWN];
    auto bb = bbs[PAWN];

    while(bb){
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        evalMg += PST::getValue(white, PAWN, pos, false);
        evalEg += PST::getValue(white, PAWN, pos, true);

        int distanceFromPromotionRev = white ? ((7 - (pos / 8)) - 1) : ((7 - (7 - (pos / 8))) - 1);
        assert(distanceFromPromotionRev >= 0);
        if((enemyPawnsBB & PAWN_PASSED_BITBOARDS[!white][pos]) == 0ULL){
            evalMg += PASSED_PAWN_BONUS;
            evalEg += PASSED_PAWN_BONUS * distanceFromPromotionRev;
        }

        // Add bonus, if pawns are in some sort of structure.
        if((bb & PAWN_FRIENDS_BITBOARDS[pos]) != 0){
            evalMg += FRIEND_PAWN_BONUS;
            evalEg += (FRIEND_PAWN_BONUS * 2);
        }

        // If pawn is isolated, subtract value from current eval.
        auto column = pos % 8;
        if((friendlyPawnsBB & PAWN_ISOLATION_BITBOARDS[column]) == 0){
            evalMg -= ISOLATED_PAWN_SUBTRACT;
            evalEg -= ISOLATED_PAWN_SUBTRACT;
        }

        // doubled/tripled pawns.
        uint64_t tmp = 0ULL;
        bit_ops::setNthBit(tmp, pos);
        if((friendlyPawnsBB & (LINE_BITBOARDS[column] ^ tmp)) != 0){
            evalMg -= STACKED_PAWN_SUBTRACT;
            evalEg -= STACKED_PAWN_SUBTRACT;
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
