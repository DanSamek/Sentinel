#ifndef SENTINEL_MOVE_H
#define SENTINEL_MOVE_H

#include <iostream>
#include <string>

struct Move {
    enum type{
        CAPTURE,
        PROMOTION,
        QUIET,
        EN_PASSANT,
        CASTLING,
        DOUBLE_PAWN_UP
    };
    enum PromotionType{
        NONE,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
    };
    int fromSq = 0;
    int toSq = 0;
    PromotionType promotionType = NONE;
    type moveType = QUIET;
    int movePiece;

    void print(){
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

    static std::string  indexToChessSquare(int index) {
        int row = 8 - index / 8;
        int col = index % 8;

        char file = 'a' + col;
        char rank = '1' + row - 1;

        std::string chessSquare = "";
        chessSquare += file;
        chessSquare += rank;

        return chessSquare;
    }
};

#endif //SENTINEL_MOVE_H
