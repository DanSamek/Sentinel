// Eval function is out of a board.cpp for "smaller" board.cpp file.
#include <board.h>
#include <cmath>

static inline constexpr int MAX_KING_DISTANCE = 14;
static inline constexpr int KING_DISTANCE_MULTIPLIER = 15;
static inline constexpr int ENEMY_KING_DISTANCE_MULTIPLIER = 7;
static inline constexpr int TWO_BISHOPS_BONUS = 30;
static inline constexpr int EVAL_DIFFERENCE_FOR_ENDGAME = 380;
static inline constexpr int CASTLING_BONUS = 17;


inline int getManhattanDist(const int posA[2], const int posB[2]){
    return std::abs(posA[0] - posB[0]) + std::abs(posA[1] - posB[1]);
}

int Board::eval() {
    // simple eval no PST for getting differences.
    int simpleEvalWhite = evalSideSimple(whitePieces);
    int simpleEvalBlack = evalSideSimple(blackPieces);
    int difference = std::abs(simpleEvalBlack - simpleEvalWhite);

    // If difference is higher, than 380 (we are winning), we turn on endgame eval -> we want more mobility for our current position. (we want to win)
    bool isEndgame = difference >= EVAL_DIFFERENCE_FOR_ENDGAME || piecesTotal <= END_GAME_PIECE_MAX;

    // PST eval for current game state.
    int whiteScore = evalSide(whitePieces, true, isEndgame);
    int blackScore = evalSide(blackPieces, false, isEndgame);

    // Eval some things for endgames.
    // Now only king distances.
    if(isEndgame){
        auto whiteKingIndex = bit_ops::bitScanForward(whitePieces[KING]);
        auto blackKingIndex = bit_ops::bitScanForward(blackPieces[KING]);

        int whiteKingPos[2] = {whiteKingIndex/8, whiteKingIndex % 8};
        int blackKingPos[2] = {blackKingIndex/8, blackKingIndex % 8};

        if(difference >= EVAL_DIFFERENCE_FOR_ENDGAME){
            // Manhattan distance between kings.
            auto distance = getManhattanDist(whiteKingPos, blackKingPos);
            int scoreToAdd = KING_DISTANCE_MULTIPLIER * (MAX_KING_DISTANCE - distance); // If we are even more winning, move king even more.

            // get, if enemy king is on edge of the board
            if(whiteScore > blackScore){
                int scoreToAddKing = BOARD_LEVELS[blackKingIndex] * ENEMY_KING_DISTANCE_MULTIPLIER;
                whiteScore += scoreToAdd + scoreToAddKing;
                blackScore -= scoreToAdd + scoreToAddKing;
            }
            else{
                int scoreToAddKing = BOARD_LEVELS[whiteKingIndex] * ENEMY_KING_DISTANCE_MULTIPLIER;
                blackScore += scoreToAdd + scoreToAddKing;
                whiteScore -= scoreToAdd + scoreToAddKing;
            }
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
    for(int j = 0; j < 6; j++){
        if(j == BISHOP) continue; // dumb fix now, do it better.
        auto bb = bbs[j];
        // TODO mobility bonus -> performance problem (??)
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

    /*
        TODO check passed pawn + structure of pawns (add some score if structure is good, subtract some score, if structure is bad)
    */
    return eval;
}