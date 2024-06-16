#include <sstream>
#include <board.h>

Board::Board() {
    initPieces(whitePieces);
    initPieces(blackPieces);
}

void Board::initPieces(uint64_t* pieces) {
    pieces[PAWN] = 0ULL;
    pieces[KNIGHT] = 0ULL;
    pieces[BISHOP] = 0ULL;
    pieces[ROOK] = 0ULL;
    pieces[QUEEN] = 0ULL;
    pieces[KING] = 0ULL;
}

void Board::loadFEN(const std::string FEN) {
    castling[0][0] = false; castling[0][1] = false; castling[1][0] = false; castling[1][1] = false;
    initPieces(whitePieces); initPieces(blackPieces);

    std::string board, whoPlayTmp, castlingRules, enPassant;

    std::istringstream iss(FEN);

    if(!(iss >> board >> whoPlayTmp >> castlingRules >> enPassant >> halfMove >> fullMove)) throw std::invalid_argument("not valid FEN.");

    // parse a board.
    int square = 0;
    for(auto character: board){
        if(isdigit(character)){
            square += (int)(character - '0');
            continue;
        } else if(character == '/') continue;
        // parse a piece
        auto color = isupper(character) ?  Bitboard::WHITE  : Bitboard::BLACK;
        auto index = pieceIndexMap[tolower(character)];

        if(color == Bitboard::WHITE)  bit_ops::setNthBit(whitePieces[index], square);
        else   bit_ops::setNthBit(blackPieces[index], square);
        square++;
    }
    whoPlay = whoPlayTmp == "w" ? true : false;
    if(enPassant != "-") enPassantSquare = (ranks[enPassant[1]] * 8) + files[enPassant[0]];
    else enPassantSquare = -1; // !

    for(auto c : castlingRules){
        if(c == '-') break;
        int white = !isupper(c);
        int index = tolower(c) == 'q' ? 0 : 1;
        castling[white][index] = true;
    }
}

const uint64_t & Board::getPieceBitboard(pieceType type, pieceColor color) const {
    return color == WHITE ? whitePieces[type] : blackPieces[type];
}

void Board::makeMove(const Move &move) {
    auto currentPieces = whoPlay ? whitePieces : blackPieces;
    auto enemyPieces = !whoPlay ? whitePieces : blackPieces;

    auto currentCastling = whoPlay ? castling[0] : castling[1];

    bool setEnPassant = false;
    int maybeCastle = (move.fromSq % 8 == 0) ? Q_CASTLE : K_CASTLE;
    std::pair<uint64_t , bool> type;
    switch (move.moveType) {
        case Move::CAPTURE:
            // simple move.
            bit_ops::setNthBit(currentPieces[move.movePiece], move.toSq);
            bit_ops::popNthBit(currentPieces[move.movePiece], move.fromSq);

            // find enemy piece and pop bit.
            type = getPieceTypeFromSQ(move.toSq, enemyPieces);
            bit_ops::popNthBit(enemyPieces[type.first], move.toSq);

            // same as in quiet.
            if(move.movePiece == ROOK && castling[!whoPlay][maybeCastle]){
                castling[!whoPlay][maybeCastle] = false;
            }
            else if(move.movePiece == KING){
                currentCastling[0] = false;
                currentCastling[1] = false;
            }
            break;
        case Move::PROMOTION:
            bit_ops::popNthBit(currentPieces[move.movePiece], move.fromSq);
            // switch for promotion type
            switch (move.promotionType) {
                case Move::QUEEN:
                    bit_ops::setNthBit(currentPieces[QUEEN], move.toSq);
                    break;
                case Move::ROOK:
                    bit_ops::setNthBit(currentPieces[ROOK], move.toSq);
                    break;
                case Move::BISHOP:
                    bit_ops::setNthBit(currentPieces[BISHOP], move.toSq);
                    break;
                case Move::KNIGHT:
                    bit_ops::setNthBit(currentPieces[KNIGHT], move.toSq);
                    break;
                default:
                    throw std::out_of_range("UNEXPECTED PROMOTION!");
            }

            // find enemy piece and pop bit.
            type =getPieceTypeFromSQ(move.toSq, enemyPieces);
            if(type.second) bit_ops::popNthBit(type.first,move.toSq);
            break;
        case Move::QUIET:
            // !!! rooks  !!! <-> disable castling.
            bit_ops::popNthBit(currentPieces[move.movePiece], move.fromSq);
            bit_ops::setNthBit(currentPieces[move.movePiece], move.toSq);
            if(move.movePiece == ROOK && castling[!whoPlay][maybeCastle]){
                castling[!whoPlay][maybeCastle] = false;
            }
            // disable castling.
            else if(move.movePiece == KING){
                currentCastling[0] = false;
                currentCastling[1] = false;
            }
            break;
        case Move::EN_PASSANT:
            bit_ops::popNthBit(currentPieces[move.movePiece], move.fromSq);
            bit_ops::setNthBit(currentPieces[move.movePiece], move.toSq);
            bit_ops::popNthBit(enemyPieces[move.movePiece], whoPlay ? move.toSq + 8 : move.toSq - 8);
            setEnPassant = true;
            break;
        case Move::CASTLING:
            // kingSide
            if(move.fromSq < move.toSq){
                bit_ops::popNthBit(currentPieces[ROOK], move.toSq + 1);
                bit_ops::setNthBit(currentPieces[ROOK], move.toSq - 1);
                bit_ops::popNthBit(currentPieces[KING], move.fromSq);
                bit_ops::setNthBit(currentPieces[KING], move.toSq);
            }
            // queenSide
            else{
                bit_ops::popNthBit(currentPieces[ROOK], move.toSq - 1);
                bit_ops::setNthBit(currentPieces[ROOK], move.toSq + 1);
                bit_ops::popNthBit(currentPieces[KING], move.fromSq);
                bit_ops::setNthBit(currentPieces[KING], move.toSq);
            }

            currentCastling[0] = 0;
            currentCastling[1] = 0;
            break;
        case Move::DOUBLE_PAWN_UP:
            bit_ops::popNthBit(currentPieces[move.movePiece], move.fromSq);
            bit_ops::setNthBit(currentPieces[move.movePiece], move.toSq);
            enPassantSquare = (move.fromSq + move.toSq) / 2;
            break;
    }

    whoPlay = !whoPlay;
    enPassantSquare = setEnPassant ? enPassantSquare : -1;
}

void Board::undoMove(const Move &move) {






    whoPlay = !whoPlay;

}

std::pair<Board::pieceType, bool> Board::getPieceTypeFromSQ(int square, const uint64_t* bbs) {
    for(int j = 0; j < 6; j++){
        if(bit_ops::getNthBit(bbs[j], square)) return {(Board::pieceType)j, true};
    }
    return {(Board::pieceType)0, false};
}

void Board::printBoard() {
    for(int rank = 0; rank < 8; rank++){
        for(int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            auto res = getPieceTypeFromSQ(square, whitePieces);
            if(res.second){
                std::cout << (char)toupper(reversedPieceIndexMap[res.first]) << " ";
                continue;
            }
            res = getPieceTypeFromSQ(square, blackPieces);
            if(res.second){
                std::cout << reversedPieceIndexMap[res.first] << " ";
            }
            else{
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}