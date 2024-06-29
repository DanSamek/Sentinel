#include <board.h>

static inline constexpr int MAX_KING_DISTANCE = 14;
static inline constexpr int KING_DISTANCE_MULTIPLIER = 10;
static inline constexpr int TWO_BISHOPS_BONUS = 30;
static inline constexpr int EVAL_DIFFERENCE_FOR_ENDGAME = 350;
static inline constexpr int CASTLING_BONUS = 17;
static inline constexpr int PASSED_PAWN_BONUS = 14;

inline int getManhattanDist(const int posA[2], const int posB[2]){
    return std::abs(posA[0] - posB[0]) + std::abs(posA[1] - posB[1]);
}

// EVAL ENDGAME PLS somehow
// Engine is brutally struggling in endgames :(
// check if isEndgame works.

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
        if(j == BISHOP) continue; // dumb fix now, do it better.
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
    //auto friendlyPawnsBB = white ? whitePieces[PAWN] : blackPieces[PAWN]; // TODO
    bb = bbs[PAWN];
    while(bb){
        auto pos = bit_ops::bitScanForwardPopLsb(bb);
        eval += PST::getValue(white, PAWN, pos, isEndgame);

        // pawn is passed, if enemyPawnBB & passBB = 0.
        if((enemyPawnsBB & PASSED_PAWN_BITBOARDS[!white][pos]) == 0ULL){
            int distanceFromPromotionRev =  white ? ((7 - (pos / 8)) - 1) : ((7 - (7 - (pos / 8))) - 1);
            assert(distanceFromPromotionRev >= 0);
            eval += piecesTotal <= END_GAME_PIECE_MAX ? (PASSED_PAWN_BONUS * distanceFromPromotionRev) : PASSED_PAWN_BONUS;
        }
        // check, if pawn is isolated (aka no pawn friends?!).
    }
    return eval;
}


void Board::initPawnEvalBBS(){
    initPassedPawnBBS();
    initIsolatedPawnBBS();
}

void  Board::initPassedPawnBBS(){
    for(int j = 0; j < 64; j++){
        PASSED_PAWN_BITBOARDS[WHITE][j] = 0ULL;
        PASSED_PAWN_BITBOARDS[BLACK][j] = 0ULL;
    }

    for(int square = 16; square < 56; square++){
        int tmp = square - 8;
        while(tmp >= 8){
            setPassedPawnBits(square, tmp, 0);
            tmp -= 8;
        }
    }

    for(int square = 8; square <= 47; square++){
        int tmp = square + 8;
        while(tmp <= 55){
            setPassedPawnBits(square, tmp, 1);
            tmp += 8;
        }
    }
}

void Board::setPassedPawnBits(int square, int tmp, int index) {
    bit_ops::setNthBit(PASSED_PAWN_BITBOARDS[index][square], tmp);
    if(square % 8 == 0 || (square + 1)% 8 != 0) bit_ops::setNthBit(PASSED_PAWN_BITBOARDS[index][square], tmp + 1);
    if((square + 1) % 8 == 0 || (square % 8) != 0) bit_ops::setNthBit(PASSED_PAWN_BITBOARDS[index][square], tmp - 1);
}

void Board::initIsolatedPawnBBS(){

}