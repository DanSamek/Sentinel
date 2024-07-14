#include <board.h>
#include <movegen.h>

static inline constexpr int MAX_KING_DISTANCE = 14;
static inline constexpr int KING_DISTANCE_MULTIPLIER = 10; // TODO buff!!
static inline constexpr int TWO_BISHOPS_BONUS = 30;
static inline constexpr int EVAL_DIFFERENCE_FOR_ENDGAME = 350;
static inline constexpr int CASTLING_BONUS = 12;
static inline constexpr int PASSED_PAWN_BONUS = 15;
static inline constexpr int ISOLATED_PAWN_SUBTRACT = 15;
static inline constexpr int FRIEND_PAWN_BONUS = 10;


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
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    2,
    4,
    5,
    8,
    9,
    11,
    12,
    13,
    13,
    13,
    13,
    13,
    13,
    13,
    13,
    13,
    13,
    13,
    13
};


inline int getManhattanDist(const int posA[2], const int posB[2]){
    return std::abs(posA[0] - posB[0]) + std::abs(posA[1] - posB[1]);
}

int Board::eval() {
    // simple eval no PST for getting differences.
    int simpleEvalWhite = evalSideSimple(whitePieces);
    int simpleEvalBlack = evalSideSimple(blackPieces);
    int difference = std::abs(simpleEvalBlack - simpleEvalWhite);

    // If difference is higher, than $EVAL_DIFFERENCE_FOR_ENDGAME (we are winning), we turn on endgame eval -> we want more mobility for our current position. (we want to win)
    bool isEndgame = difference >= EVAL_DIFFERENCE_FOR_ENDGAME || piecesTotal <= END_GAME_PIECE_MAX;


    uint64_t all = 0ULL;
    for(int j = 0; j < 6; j++){
        all |= whitePieces[j];
        all |= blackPieces[j];
    }

    int whiteScore = evalSide(whitePieces, true, isEndgame, all);
    int blackScore = evalSide(blackPieces, false, isEndgame, all);

    // Eval some things for endgames.
    // Now only king distances.
    // only endgames
    if(piecesTotal <= END_GAME_PIECE_MAX){
        auto whiteKingIndex = bit_ops::bitScanForward(whitePieces[KING]);
        auto blackKingIndex = bit_ops::bitScanForward(blackPieces[KING]);

        int whiteKingPos[2] = {whiteKingIndex/8, whiteKingIndex % 8};
        int blackKingPos[2] = {blackKingIndex/8, blackKingIndex % 8};

        // Manhattan distance between kings.
        auto distance = getManhattanDist(whiteKingPos, blackKingPos);
        int scoreToAdd = KING_DISTANCE_MULTIPLIER * (MAX_KING_DISTANCE - distance);

        // Add bonus for side, if king is closer to enemy king
        if(whiteScore > blackScore){
            whiteScore += scoreToAdd;
        }
        else{
            blackScore += scoreToAdd;
        }
    }

    // add bonus, if castling is still possible
    // if king is attacked, dont move king, just try block.
    whiteScore += (castling[pieceColor::WHITE][0] || castling[pieceColor::WHITE][1]) ? CASTLING_BONUS : 0;
    blackScore += (castling[pieceColor::WHITE][0] || castling[pieceColor::WHITE][1]) ? CASTLING_BONUS : 0;

    return (whiteScore - blackScore) * (whoPlay ? 1 : -1);
}

// simple eval of current position
int Board::evalSideSimple(uint64_t *bbs) const{
    int eval = 0;
    for(int j = 1; j < 6; j++){
        auto bb = bbs[j];
        while(bb){
            bit_ops::bitScanForwardPopLsb(bb);
            eval += PST::PIECE_EVAL_MG[j];
        }
    }
    return eval;
}

int Board::evalSide(uint64_t *bbs, bool white, bool isEndgame, const uint64_t& all) const{
    int eval = 0;

    eval += evalPawns(bbs, white, isEndgame);

    eval += evalBishops(bbs, white, isEndgame, all);
    eval += evalKnights(bbs, white, isEndgame, all);
    eval += evalRooks(bbs, white, isEndgame, all);
    eval += evalQueens(bbs, white, isEndgame, all);
    eval += evalKing(bbs, white, isEndgame, all);

    return eval;
}


int Board::evalKnights(uint64_t *bbs, bool white, bool isEndgame, const uint64_t& all) const {
    // count number of bishops, add bonus.
    int eval = 0;
    auto bb = bbs[KNIGHT];
    while(bb){
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        eval += PST::getValue(white, KNIGHT, pos, isEndgame);

        auto attacks = Movegen::KNIGHT_MOVES[pos];
        auto attackCount = bit_ops::countBits(attacks);
        eval += KNIGHT_MOBILITY_BONUS[attackCount];
    }

    return eval;
}

int Board::evalRooks(uint64_t *bbs, bool white, bool isEndgame, const uint64_t& all) const {
    // count number of bishops, add bonus.
    int eval = 0;
    auto bb = bbs[ROOK];
    while(bb){
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        eval += PST::getValue(white, ROOK, pos, isEndgame);

        auto attacks = Magics::getRookMoves(all, pos);
        auto attackCount = bit_ops::countBits(attacks);
        eval += ROOK_MOBILITY_BONUS[attackCount];
    }

    return eval;
}


int Board::evalQueens(uint64_t *bbs, bool white, bool isEndgame, const uint64_t& all) const {
    // count number of bishops, add bonus.
    int eval = 0;
    auto bb = bbs[QUEEN];
    while(bb){
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        eval += PST::getValue(white, QUEEN, pos, isEndgame);

        auto attacks = Magics::getRookMoves(all, pos);
        attacks |= Magics::getBishopMoves(all, pos);
        auto attackCount = bit_ops::countBits(attacks);
        eval += QUEEN_MOBILITY_BONUS[attackCount];
        assert( QUEEN_MOBILITY_BONUS[attackCount] < 20);
    }

    return eval;
}


int Board::evalKing(uint64_t *bbs, bool white, bool isEndgame, const uint64_t& all) const {
    // count number of bishops, add bonus.
    int eval = 0;
    auto bb = bbs[KING];
    while(bb){
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        eval += PST::getValue(white, KING, pos, isEndgame);
        // TODO virtual mobility for not isEndgame
    }
    return eval;
}


int Board::evalBishops(uint64_t *bbs, bool white, bool isEndgame, const uint64_t& all) const {
    // count number of bishops, add bonus.
    int eval = 0;
    int bishopCount = 0;
    auto bb = bbs[BISHOP];
    while(bb){
        bishopCount++;
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        eval += PST::getValue(white, BISHOP, pos, isEndgame);
        auto attacks = Magics::getBishopMoves(all, pos);
        auto attackCount = bit_ops::countBits(attacks);
        eval += BISHOP_MOBILITY_BONUS[attackCount];
    }
    eval += bishopCount >= 2 ? TWO_BISHOPS_BONUS : 0; // Rays goes brr.

    return eval;
}

int Board::evalPawns(uint64_t *bbs, bool white, bool isEndgame) const{
    // Pawn eval.
    int eval = 0;
    auto enemyPawnsBB = white ? blackPieces[PAWN] : whitePieces[PAWN];
    auto friendlyPawnsBB = white ? whitePieces[PAWN] : blackPieces[PAWN];
    auto bb = bbs[PAWN];

    while(bb){
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        eval += PST::getValue(white, PAWN, pos, isEndgame);
        int distanceFromPromotionRev = white ? ((7 - (pos / 8)) - 1) : ((7 - (7 - (pos / 8))) - 1);
        assert(distanceFromPromotionRev >= 0);
        if((enemyPawnsBB & PAWN_PASSED_BITBOARDS[!white][pos]) == 0ULL){
            eval += piecesTotal <= END_GAME_PIECE_MAX ? (PASSED_PAWN_BONUS * distanceFromPromotionRev) : PASSED_PAWN_BONUS;
        }

        // Add bonus, if pawns are in some sort of structure.
        if((bb & PAWN_FRIENDS_BITBOARDS[pos]) != 0){
            eval += isEndgame ? (FRIEND_PAWN_BONUS * 2) : FRIEND_PAWN_BONUS;
        }

        // If pawn is isolated, subtract value from current eval.
        auto column = pos % 8;
        if((friendlyPawnsBB & PAWN_ISOLATION_BITBOARDS[column]) == 0){
            eval -= ISOLATED_PAWN_SUBTRACT;
        }
    }
    return eval;
}

void Board::initPawnEvalBBS(){
    initPassedPawnBBS();
    initPawnIsolationBBS();
    initPawnFriendsBBS();
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
