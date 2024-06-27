// Eval function is out of a board.cpp

#include <board.h>

int Board::eval() {
    // read all pieces on the board.
    // arrays for all pieces, (square on board).

    // simple eval if we are winning (difference is >= 500).
    // if yes, we can turn on endgame eval

    // if are winning, we want our king go near to an an enemy king to help with a checkmate bonus smaller distances between our king and enemy king.
    // whoplay.


    int whiteScore = evalSide(whitePieces, true);
    int blackScore = evalSide(blackPieces, false);


    return (whiteScore - blackScore) * (whoPlay ? 1 : -1);  
}


int Board::evalSide(uint64_t *bbs, bool white) const{
    int eval = 0;
    // pawns eval differently TODO
    for(int j = 0; j < 6; j++){
        auto bb = bbs[j];
        while(bb){
            auto pos = bit_ops::bitScanForwardPopLsb(bb);
            eval += PST::getValue(white, j, pos, piecesTotal > END_GAME_PIECE_MAX);
        }
    }

    // lets check passed pawn + structure of pawns. TODO
    // if we are getting into a winning endgame (middlegame), we want our pawns move from starting squares. TODO

    return eval;
}