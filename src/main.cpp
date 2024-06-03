#include "bitboard.h"
#include "movegen.h"

int main(){
    Movegen::initTables();
    Bitboard tmp;
    for(auto item: Movegen::KING_MOVES){
        tmp.value = item;
        tmp.printBoard();
    }
    return 1;
}