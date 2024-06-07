#include <tests/boardTests.cpp>
#include <magics.h>

int main(){
    boardTests::testBoard();
    Magics::init();


    Board board;
    board.loadFEN("rnbqkbnr/Pppppppp/8/K7/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    auto friendlyBits = board.whoPlay ? board.whitePieces : board.blackPieces;
    auto enemyBits = board.whoPlay ? board.blackPieces : board.whitePieces;

    // pinning logic TODO!
    Bitboard friendlyMerged = friendlyBits[0] | friendlyBits[1] | friendlyBits[2] | friendlyBits[3] | friendlyBits[4] | friendlyBits[5];
    Bitboard enemyMerged = enemyBits[0] | enemyBits[1] | enemyBits[2] | enemyBits[3] | enemyBits[4] | enemyBits[5];
    Bitboard all = friendlyMerged | enemyMerged;
    all.printBoard();

    Bitboard result = Magics::getSlidingMoves(all.value, 43, true);
    result.printBoard();
    result = Magics::getSlidingMoves(all.value, 43, false);
    result.printBoard();

    auto tmp = result & enemyBits[5]; // is in ray.
    if(result.value){
        // generate all rays to all directions and and if != 0 king is here => pin a piece -> and with attack mask.
        tmp.printBoard();
    }
    return 0;
}