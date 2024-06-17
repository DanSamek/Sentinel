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

void Board::makeMove(const Move &move, int depth) {
    auto currentPieces = whoPlay ? whitePieces : blackPieces;
    auto enemyPieces = !whoPlay ? whitePieces : blackPieces;

    auto currentCastling = whoPlay ? castling[0].data() : castling[1].data();
    auto enemyCastling = whoPlay ? castling[1].data() : castling[0].data();

    bool setEnPassant = false;

    State currentState;
    currentState.enPassantSquare = enPassantSquare;
    currentState.castling = castling;

    std::pair<uint64_t , bool> type;
    switch (move.moveType) {
        case Move::CAPTURE:
            // simple move.
            bit_ops::setNthBit(currentPieces[move.movePiece], move.toSq);
            bit_ops::popNthBit(currentPieces[move.movePiece], move.fromSq);

            // find enemy piece and pop bit.
            type = getPieceTypeFromSQ(move.toSq, enemyPieces);
            bit_ops::popNthBit(enemyPieces[type.first], move.toSq);

            currentState.captureType = type.first;
            // !! castling !! <-> for enemy !!
            if(type.first == ROOK){
                // black enemy
                if(whoPlay){
                    if(move.toSq == 0) enemyCastling[Q_CASTLE] = false;
                    else if(move.toSq == 7) enemyCastling[K_CASTLE] = false;
                }
                // white enemy
                else{
                    if(move.toSq == 56) enemyCastling[Q_CASTLE] = false;
                    else if(move.toSq == 63) enemyCastling[K_CASTLE] = false;
                }
            }
            // same as in quiet.
            if(move.movePiece == ROOK){
                if(whoPlay){
                    if(move.fromSq == 56) currentCastling[Q_CASTLE] = false;
                    else if(move.fromSq== 63) currentCastling[K_CASTLE] = false;
                }
                else{
                    if(move.fromSq == 0) currentCastling[Q_CASTLE] = false;
                    else if(move.fromSq == 7) currentCastling[K_CASTLE] = false;
                }
            }
            if(move.movePiece == KING){
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
            type = getPieceTypeFromSQ(move.toSq, enemyPieces);
            if(type.second){
                bit_ops::popNthBit(enemyPieces[type.first], move.toSq);
                currentState.captureType = type.first;
                // pawn can capture rook -> no castling.
                if(type.first == ROOK){
                    // black enemy
                    if(whoPlay){
                        if(move.toSq == 0) enemyCastling[Q_CASTLE] = false;
                        else if(move.toSq == 7) enemyCastling[K_CASTLE] = false;
                    }
                        // white enemy
                    else{
                        if(move.toSq == 56) enemyCastling[Q_CASTLE] = false;
                        else if(move.toSq == 63) enemyCastling[K_CASTLE] = false;
                    }
                }
            }
            break;
        case Move::QUIET:
            // !!! rooks  !!! <-> disable castling.
            bit_ops::popNthBit(currentPieces[move.movePiece], move.fromSq);
            bit_ops::setNthBit(currentPieces[move.movePiece], move.toSq);
            if(move.movePiece == ROOK){
                if(whoPlay){
                    if(move.fromSq == 56) currentCastling[Q_CASTLE] = false;
                    else if(move.fromSq== 63) currentCastling[K_CASTLE] = false;
                }
                else{
                    if(move.fromSq == 0) currentCastling[Q_CASTLE] = false;
                    else if(move.fromSq == 7) currentCastling[K_CASTLE] = false;
                }
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
                bit_ops::popNthBit(currentPieces[ROOK], move.toSq - 2);
                bit_ops::setNthBit(currentPieces[ROOK], move.toSq + 1);
                bit_ops::popNthBit(currentPieces[KING], move.fromSq);
                bit_ops::setNthBit(currentPieces[KING], move.toSq);
            }
            currentCastling[0] = false;
            currentCastling[1] = false;
            break;
        case Move::DOUBLE_PAWN_UP:
            bit_ops::popNthBit(currentPieces[move.movePiece], move.fromSq);
            bit_ops::setNthBit(currentPieces[move.movePiece], move.toSq);
            enPassantSquare = (move.fromSq + move.toSq) / 2;
            setEnPassant = true;
            break;
    }

    // save state to a current depth
    STACK[depth] = std::move(currentState);

    whoPlay = !whoPlay;
    enPassantSquare = setEnPassant ? enPassantSquare : -1;
}

void Board::undoMove(const Move &move, int depth) {
    auto prevState = std::move(STACK[depth]);
    whoPlay = !whoPlay;

    auto currentPieces = whoPlay ? whitePieces : blackPieces;
    auto enemyPieces = !whoPlay ? whitePieces : blackPieces;

    castling = std::move(prevState.castling); // set prev castling rules.
    enPassantSquare = prevState.enPassantSquare; // reset of an en passant square.

    switch (move.moveType) {
        case Move::CAPTURE:
            bit_ops::setNthBit(currentPieces[move.movePiece], move.fromSq);
            bit_ops::popNthBit(currentPieces[move.movePiece], move.toSq);

            bit_ops::setNthBit(enemyPieces[prevState.captureType], move.toSq);
            break;
        case Move::PROMOTION:
            // undo a promotion
            bit_ops::setNthBit(currentPieces[move.movePiece], move.fromSq);
            switch (move.promotionType) {
                case Move::QUEEN:
                    bit_ops::popNthBit(currentPieces[QUEEN], move.toSq);
                    break;
                case Move::ROOK:
                    bit_ops::popNthBit(currentPieces[ROOK], move.toSq);
                    break;
                case Move::BISHOP:
                    bit_ops::popNthBit(currentPieces[BISHOP], move.toSq);
                    break;
                case Move::KNIGHT:
                    bit_ops::popNthBit(currentPieces[KNIGHT], move.toSq);
                    break;
                default:
                    throw std::out_of_range("UNEXPECTED UNDER-PROMOTION!");
            }
            // undo capture.
            if(prevState.captureType != -1) bit_ops::setNthBit(enemyPieces[prevState.captureType], move.toSq);
            break;
        case Move::QUIET:
            bit_ops::popNthBit(currentPieces[move.movePiece], move.toSq);
            bit_ops::setNthBit(currentPieces[move.movePiece], move.fromSq);
            break;
        case Move::EN_PASSANT:
            // move back pawn, "respawn" enemyPawn.
            bit_ops::setNthBit(enemyPieces[move.movePiece], whoPlay ? move.toSq + 8 : move.toSq - 8);
            bit_ops::setNthBit(currentPieces[move.movePiece], move.fromSq);
            bit_ops::popNthBit(currentPieces[move.movePiece], move.toSq);
            break;
        case Move::CASTLING:
            // move rook and king to original squares.
            // kingSide
            if(move.fromSq < move.toSq){
                bit_ops::popNthBit(currentPieces[ROOK], move.toSq - 1);
                bit_ops::setNthBit(currentPieces[ROOK], move.toSq + 1);
                bit_ops::popNthBit(currentPieces[KING], move.toSq);
                bit_ops::setNthBit(currentPieces[KING], move.fromSq);
            }
            // queenSide
            else{
                bit_ops::popNthBit(currentPieces[ROOK], move.toSq + 1);
                bit_ops::setNthBit(currentPieces[ROOK], move.toSq - 2);
                bit_ops::popNthBit(currentPieces[KING], move.toSq);
                bit_ops::setNthBit(currentPieces[KING], move.fromSq);
            }
            break;
        case Move::DOUBLE_PAWN_UP:
            bit_ops::setNthBit(currentPieces[move.movePiece], move.fromSq);
            bit_ops::popNthBit(currentPieces[move.movePiece], move.toSq);
            break;
    }
}

std::pair<Board::pieceType, bool> Board::getPieceTypeFromSQ(int square, const uint64_t* bbs) {
    for(int j = 0; j < 6; j++){
        if(bit_ops::getNthBit(bbs[j], square)) return {(Board::pieceType)j, true};
    }
    return {(Board::pieceType)0, false};
}

void Board::printBoard() {
    std::vector<bool> seen(64, false);
    for(int rank = 0; rank < 8; rank++){
        for(int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            auto res = getPieceTypeFromSQ(square, whitePieces);
            if(res.second){
                if(seen[square]) throw "??";
                seen[square] = true;
                std::cout << (char)toupper(reversedPieceIndexMap[res.first]) << " ";
                continue;
            }
            res = getPieceTypeFromSQ(square, blackPieces);
            if(res.second){
                if(seen[square]) throw "??";
                seen[square] = true;
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