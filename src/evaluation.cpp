#include <board.h>
#include <movegen.h>

static inline constexpr int MAX_KING_DISTANCE = 14;
static inline constexpr int KING_DISTANCE_MULTIPLIER = 10;
static inline constexpr int TWO_BISHOPS_BONUS = 30;
static inline constexpr int EVAL_DIFFERENCE_FOR_ENDGAME = 350;
static inline constexpr int CASTLING_BONUS = 12;
static inline constexpr int PASSED_PAWN_BONUS = 15;
static inline constexpr int ISOLATED_PAWN_SUBTRACT = 15;
static inline constexpr int FRIEND_PAWN_BONUS = 10;

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

    // PST eval for current game state.
    int whiteScore = evalSide(whitePieces, true, isEndgame);
    int blackScore = evalSide(blackPieces, false, isEndgame);

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

int Board::evalSide(uint64_t *bbs, bool white, bool isEndgame) const{
    int eval = 0;
    for(int j = 1; j < 6; j++){
        if(j == BISHOP ) continue; // dumb fix now, do it better.
        auto bb = bbs[j];
        while(bb){
            auto pos = bit_ops::bitScanForwardPopLsb(bb);
            eval += PST::getValue(white, j, pos, isEndgame);
        }
    }
    // count number of bishops, add bonus.
    int bishopCount = 0;
    auto bb = bbs[BISHOP];
    while(bb){
        bishopCount++;
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        eval += PST::getValue(white, BISHOP, pos, isEndgame);
    }

    eval += bishopCount >= 2 ? TWO_BISHOPS_BONUS : 0; // Rays goes brr.

    // Pawn eval.
    auto enemyPawnsBB = white ? blackPieces[PAWN] : whitePieces[PAWN];
    auto friendlyPawnsBB = white ? whitePieces[PAWN] : blackPieces[PAWN];
    bb = bbs[PAWN];

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
