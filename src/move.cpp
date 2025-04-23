#include <move.h>

namespace Sentinel{

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

    bool Move::operator ==(const Move &other) const {
        return other.fromSq == fromSq && other.toSq == toSq && other.movePiece == movePiece && other.moveType == moveType && other.promotionType == promotionType;
    }

    bool Move::isCapture() const {
        return moveType == CAPTURE || moveType == PROMOTION_CAPTURE;
    }

    bool Move::isPromotion() const {
        return moveType == PROMOTION;
    }
}