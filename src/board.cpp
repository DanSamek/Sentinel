#include <sstream>
#include <board.h>

Board::Board() {
    initPieces(whitePieces, Bitboard::WHITE);
    initPieces(blackPieces, Bitboard::BLACK);
}

void Board::initPieces(std::vector<Bitboard>& pieces, Bitboard::pieceColor color) {
    pieces = std::vector<Bitboard>(6);
    // same as Bitboard::pieceType...
    pieces[0] = Bitboard(0, Bitboard::PAWN, color);
    pieces[1] = Bitboard(0, Bitboard::KNIGHT, color);
    pieces[2] = Bitboard(0, Bitboard::BISHOP, color);
    pieces[3] = Bitboard(0, Bitboard::ROOK, color);
    pieces[4] = Bitboard(0, Bitboard::QUEEN, color);
    pieces[5] = Bitboard(0, Bitboard::KING, color);
}

void Board::loadFEN(const std::string FEN) {
    castling[0][0] = false; castling[0][1] = false; castling[1][0] = false; castling[1][1] = false;
    initPieces(whitePieces, Bitboard::WHITE); initPieces(blackPieces, Bitboard::BLACK);

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

        if(color == Bitboard::WHITE) whitePieces[index].setNthBit(square);
        else blackPieces[index].setNthBit(square);
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

const Bitboard& Board::getPieceBitboard(Bitboard::pieceType type, Bitboard::pieceColor color) const {
    return color == Bitboard::WHITE ? whitePieces[type] : blackPieces[type];
}