#include "magics.h"
#include "bitboard.h"
#include "movegen.h"

int main(){
    Bitboard b;
    b.value = 14951849539607157325ull;
    b.printBoard();
    magics::init();
    auto moves = magics::getSlidingMoves(b.value, 5, true);

    b.value = moves;
    b.printBoard();

    return 0;
}