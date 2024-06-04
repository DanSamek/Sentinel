#include "magics.h"
#include "bitboard.h"
#include "movegen.h"

int main(){
    /*
    Bitboard b;
    b.value = 14951849539607157325ull;
    b.printBoard();
    Magics::init();
    auto moves = Magics::getSlidingMoves(b.value, 5, true);

    b.value = moves;
    b.printBoard();
    */

    Movegen::initTables();
    Bitboard a,b,c;
    a.setNthBit(9);
    a.setNthBit(43);
    a.setNthBit(15);
    a.color = Bitboard::BLACK;
    b.value = 0;
    b.setNthBit(16);
    b.setNthBit(17+8);
    b.setNthBit(18);
    auto all = a | b;
    all.printBoard();

    Movegen::generatePawnMoves(a, a, b, all);
    Movegen::generatePawnMoves(b, b, a, all);

    return 0;
}