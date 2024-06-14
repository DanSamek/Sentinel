#include <tests/boardTests.cpp>
#include <magics.h>
#include <movegen.h>

int main(){
    boardTests::testBoard();
    Magics::init();
    Movegen::initTables();

    Board board;
    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");


    int tc = 10'000'000;
    while(tc--){
        Movegen::generateMoves(board);
    }
    return 1;

    auto friendlyBits = board.whoPlay ? board.whitePieces : board.blackPieces;
    auto enemyBits = board.whoPlay ? board.blackPieces : board.whitePieces;

    Bitboard friendlyMerged = friendlyBits[0] | friendlyBits[1] | friendlyBits[2] | friendlyBits[3] | friendlyBits[4] | friendlyBits[5];
    Bitboard enemyMerged = enemyBits[0] | enemyBits[1] | enemyBits[2] | enemyBits[3] | enemyBits[4] | enemyBits[5];

    //friendlyMerged.printBoard();

    Bitboard all = friendlyMerged | enemyMerged;
    //all.printBoard();
/*
    int tc = 10'000'000;
    while(tc--){
        auto [chCnt, bit]  = Movegen::getKingChecksAndAttacks(friendlyBits[0], enemyBits, all.value, false);
    }
    */

    //Bitboard b(bit); b.printBoard();


    board.loadFEN("8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 w - - 99 50");
    friendlyBits = board.whoPlay ? board.whitePieces : board.blackPieces;
    enemyBits = board.whoPlay ? board.blackPieces : board.whitePieces;

    friendlyMerged = friendlyBits[0] | friendlyBits[1] | friendlyBits[2] | friendlyBits[3] | friendlyBits[4] | friendlyBits[5];
    enemyMerged = enemyBits[0] | enemyBits[1] | enemyBits[2] | enemyBits[3] | enemyBits[4] | enemyBits[5];
    enemyMerged.printBoard();

    all = friendlyMerged | enemyMerged;

    //int tc = 100'000'000;
    /*
    while(tc--){
        auto [chCnt, bit]  = Movegen::getKingChecksAndAttacks(friendlyBits[0], enemyBits, all.value, false);
    }
    auto [a, b] = Movegen::getKingChecksAndAttacks(friendlyBits[0], enemyBits, all.value, false);

    Bitboard bb(b); bb.printBoard();
     */

    return 0;
}