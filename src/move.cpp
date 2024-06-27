#include <move.h>

void Move::print() {
    std::cout << indexToChessSquare(fromSq) << indexToChessSquare(toSq);
    switch (promotionType) {
        case KNIGHT:
            std::cout << "n";
            break;
        case BISHOP:
            std::cout << "b";
            break;
        case ROOK:
            std::cout << "r";
            break;
        case QUEEN:
            std::cout << "q";
            break;
        default:
            break;
    }
    std::cout<< std::endl;
}


std::string Move::indexToChessSquare(int index) {
    int row = 8 - index / 8;
    int col = index % 8;

    char file = 'a' + col;
    char rank = '1' + row - 1;

    std::string chessSquare = "";
    chessSquare += file;
    chessSquare += rank;

    return chessSquare;
}