#include <tests/boardTests.cpp>
#include <magics.h>
#include <movegen.h>

int main(){
    boardTests::testBoard();
    Magics::init();


    Board board;
    board.loadFEN("rnbqkbnr/Pppppppp/8/K7/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
    auto friendlyBits = board.whoPlay ? board.whitePieces : board.blackPieces;
    auto enemyBits = board.whoPlay ? board.blackPieces : board.whitePieces;

    // pinning logic TODO!
    Bitboard friendlyMerged = friendlyBits[0] | friendlyBits[1] | friendlyBits[2] | friendlyBits[3] | friendlyBits[4] | friendlyBits[5];
    Bitboard enemyMerged = enemyBits[0] | enemyBits[1] | enemyBits[2] | enemyBits[3] | enemyBits[4] | enemyBits[5];

    friendlyMerged.printBoard();

    Bitboard all = friendlyMerged | enemyMerged;
    all.printBoard();
    Movegen::initTables();

    int tc = 10'000'000;
    while(tc--){
        auto [chCnt, bit]  = Movegen::getKingChecksAndAttacks(friendlyBits[0], enemyBits, all.value, false);
    }
    auto [chCnt, bit]  = Movegen::getKingChecksAndAttacks(friendlyBits[0], enemyBits, all.value, false);
    Bitboard b(bit); b.printBoard();




    return 0;
}