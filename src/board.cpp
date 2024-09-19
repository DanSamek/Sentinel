#include <sstream>
#include "board.h"
#include "zobrist.h"
#include "movegen.h"
#include <iostream>
#include <vector>

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
    // reset repetitions.
    halfMove = 0;
    repetitionIndex = 0;

    castling[0][0] = false; castling[0][1] = false; castling[1][0] = false; castling[1][1] = false;
    initPieces(whitePieces); initPieces(blackPieces);

    std::string board, whoPlayTmp, castlingRules, enPassant;

    std::istringstream iss(FEN);

    if(!(iss >> board >> whoPlayTmp >> castlingRules >> enPassant >> halfMove >> fullMove)) {
        std::cout << "FEN is not valid!"<< std::endl;
        return;
    }
    ply = 0;

    // parse a _board.
    int square = 0;
    for(auto character: board){
        if(isdigit(character)){
            square += (int)(character - '0');
            continue;
        } else if(character == '/') continue;

        // parse a piece
        auto color = isupper(character) ?  WHITE  : BLACK;
        auto index = pieceIndexMap[tolower(character)];

        if(color == WHITE)  bit_ops::setNthBit(whitePieces[index], square);
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
    // get zobrist key.
    zobristKey = Zobrist::getHash(*this);
    threeFoldRepetition[repetitionIndex] = zobristKey;
}

const uint64_t & Board::getPieceBitboard(pieceType type, pieceColor color) const {
    return color == WHITE ? whitePieces[type] : blackPieces[type];
}

bool Board::makeMove(const Move &move) {
    // validate move if king is checked, return false

    auto currentPieces = whoPlay ? whitePieces : blackPieces;
    auto enemyPieces = !whoPlay ? whitePieces : blackPieces;

    auto currentCastling = whoPlay ? castling[0].data() : castling[1].data();
    auto enemyCastling = whoPlay ? castling[1].data() : castling[0].data();

    bool setEnPassant = false;

    State currentState{-1,enPassantSquare, castling, halfMove, zobristKey, fullMove};

    std::pair<uint64_t , bool> type;
    switch (move.moveType) {
        case Move::CAPTURE:
            // simple move.
            moveAPiece(currentPieces, move.movePiece, move.fromSq, move.toSq);

            // find enemy piece and pop bit.
            type = getPieceTypeFromSQ(move.toSq, enemyPieces);
            bit_ops::popNthBit(enemyPieces[type.first], move.toSq);

            currentState.captureType = type.first;
            // !! castling !! <-> for enemy !!

            handleEnemyCastling(type, move, whoPlay, enemyCastling);
            handleCastling(move, whoPlay, currentCastling);
            halfMove = 0;
            break;
        case Move::PROMOTION:
        case Move::PROMOTION_CAPTURE:
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

            // promo capture.
            if(move.moveType != Move::PROMOTION_CAPTURE) break;

            // find enemy piece and pop bit.
            type = getPieceTypeFromSQ(move.toSq, enemyPieces);
            if(type.second){
                bit_ops::popNthBit(enemyPieces[type.first], move.toSq);
                currentState.captureType = type.first;
                // pawn can capture rook -> no castling.
                handleEnemyCastling(type, move, whoPlay, enemyCastling);
            }
            halfMove = 0;
            break;
        case Move::QUIET:
            // !!! rooks  !!! <-> disable castling.
            moveAPiece(currentPieces, move.movePiece, move.fromSq, move.toSq);
            handleCastling(move, whoPlay, currentCastling);

            if(move.movePiece == Board::PAWN) halfMove = 0;
            else halfMove++;
            break;
        case Move::EN_PASSANT:
            moveAPiece(currentPieces, move.movePiece, move.fromSq, move.toSq);
            bit_ops::popNthBit(enemyPieces[move.movePiece], whoPlay ? move.toSq + 8 : move.toSq - 8);

            halfMove = 0;
            break;
        case Move::CASTLING:
            // kingSide
            moveAPiece(currentPieces, KING, move.fromSq, move.toSq);
            if(move.fromSq < move.toSq){
                moveAPiece(currentPieces, ROOK, move.toSq + 1, move.toSq - 1);
            }
            // queenSide
            else{
                moveAPiece(currentPieces, ROOK, move.toSq - 2, move.toSq + 1);
            }
            currentCastling[0] = false;
            currentCastling[1] = false;

            halfMove++;
            break;
        case Move::DOUBLE_PAWN_UP:
            moveAPiece(currentPieces, move.movePiece, move.fromSq, move.toSq);
            enPassantSquare = (move.fromSq + move.toSq) / 2;
            setEnPassant = true;

            halfMove = 0;
            break;
    }

    // validate castling.
    if(move.moveType == Move::CASTLING){
        int from = move.fromSq, to = move.toSq;
        if(move.fromSq > move.toSq) std::swap(from, to);

        for(int j = from; j <= to; j++){
            if(isSquareAttacked(j, !whoPlay)){
                push(setEnPassant, currentState);
                undoMove(move);
                return false;
            }
        }
    }
    else if(isSquareAttacked(bit_ops::bitScanForward(currentPieces[KING]), !whoPlay)){
        push(setEnPassant, currentState);
        undoMove(move);
        return false;
    }

    fullMove += !whoPlay ? 1 : 0;

    // update and add to a move "stack".
    // save state to a current depth
    switch (move.moveType) {
        case Move::CAPTURE:
            Zobrist::updateHashMove(zobristKey, move,*this, currentState);
            break;
        case Move::PROMOTION:
        case Move::PROMOTION_CAPTURE:
            Zobrist::updatePromotionHash(zobristKey, move, *this, currentState);
            break;
        case Move::EN_PASSANT:
            Zobrist::updateEnPassantHash(zobristKey, move,*this, currentState);
            break;
        case Move::CASTLING:
            Zobrist::updateCastlingHash(zobristKey, move,*this, currentState);
            break;
        case Move::DOUBLE_PAWN_UP:
            Zobrist::updateHashMove(zobristKey, move,*this, currentState);
            break;
        case Move::QUIET:
            Zobrist::updateHashMove(zobristKey, move,*this, currentState);
            break;
    }

    push(setEnPassant, currentState);
    threeFoldRepetition[repetitionIndex] = zobristKey; // just add key to an array.
    return true;
}

void Board::push(bool setEnPassant, State &currentState) {
    if(!setEnPassant && currentState.enPassantSquare != -1){
        // remove current en-passant.
        zobristKey ^= Zobrist::enPassantTable[currentState.enPassantSquare % 8];
        // set no enPassant.
        zobristKey ^= Zobrist::noEnPassant;
    }

    ply++;
    repetitionIndex++;
    STACK[ply] = std::move(currentState);

    whoPlay = !whoPlay;
    enPassantSquare = setEnPassant ? enPassantSquare : -1;
}

void Board::undoMove(const Move &move) {
    auto prevState = std::move(STACK[ply]);
    whoPlay = !whoPlay;
    repetitionIndex--;

    auto currentPieces = whoPlay ? whitePieces : blackPieces;
    auto enemyPieces = !whoPlay ? whitePieces : blackPieces;

    castling = std::move(prevState.castling); // set prev castling rules.
    enPassantSquare = prevState.enPassantSquare; // reset of an en passant square.

    zobristKey = prevState.zobristHash;
    halfMove = prevState.halfMove;
    fullMove = prevState.fullMove;

    switch (move.moveType) {
        case Move::CAPTURE:
            moveAPiece(currentPieces, move.movePiece, move.toSq, move.fromSq);
            bit_ops::setNthBit(enemyPieces[prevState.captureType], move.toSq);
            break;
        case Move::PROMOTION:
        case Move::PROMOTION_CAPTURE:
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
            moveAPiece(currentPieces, move.movePiece, move.toSq, move.fromSq);
            break;
        case Move::EN_PASSANT:
            // move back pawn, "respawn" enemyPawn.
            bit_ops::setNthBit(enemyPieces[move.movePiece], whoPlay ? move.toSq + 8 : move.toSq - 8);
            moveAPiece(currentPieces, move.movePiece, move.toSq, move.fromSq);
            break;
        case Move::CASTLING:
            // move rook and king to original squares.
            // kingSide
            moveAPiece(currentPieces, KING, move.toSq, move.fromSq);
            if(move.fromSq < move.toSq){
                moveAPiece(currentPieces, ROOK, move.toSq - 1, move.toSq + 1);
            }
            // queenSide
            else{
                moveAPiece(currentPieces, ROOK, move.toSq + 1, move.toSq - 2);
            }
            break;
        case Move::DOUBLE_PAWN_UP:
            moveAPiece(currentPieces, move.movePiece, move.toSq, move.fromSq);
            break;
    }
    ply--;
}

void Board::printBoard() const{
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


bool Board::isDraw(){
    if(isThreeFoldRepetition()) return true;

    if(halfMove >= 100) return true;

    // count material from bbs
    return isInsufficientMaterial(whitePieces) && isInsufficientMaterial(blackPieces);
}

bool Board::isThreeFoldRepetition() const {
    int cnt = 0;
    for(int j = 0; j < repetitionIndex; j++){
        if(threeFoldRepetition[j] == zobristKey) cnt++;
        if(cnt == 2) return true;
    }
    return false;
}


bool Board::isInsufficientMaterial(uint64_t* bbs) const{
    // pawns, rooks, queens.
    if(bbs[PAWN] || bbs[ROOK] || bbs[QUEEN]) return false;

    auto bb = bbs[BISHOP];
    int bishopCnt = bit_ops::countBits(bb);
    if(bishopCnt >= 2) return false;

    bb = bbs[KNIGHT];
    int knightCnt = bit_ops::countBits(bb);
    if(knightCnt >= 2) return false;

    if (knightCnt == 1 && bishopCnt == 1) return false;

    return true;
}

bool Board::isSquareAttacked(int square, bool isWhiteEnemy) {
    auto enemies = isWhiteEnemy ? whitePieces : blackPieces;
    auto current = isWhiteEnemy ? blackPieces : whitePieces;
    uint64_t all = 0ULL;
    for(int j = 0; j < 6; j++){
        all |= enemies[j];
        all |= current[j];
    }
    if(Magics::getRookMoves(all, square) & (enemies[Board::ROOK] | enemies[Board::QUEEN])) return true;

    if(Magics::getBishopMoves(all, square) & (enemies[Board::BISHOP] | enemies[Board::QUEEN])) return true;

    if(Movegen::KNIGHT_MOVES[square] & enemies[Board::KNIGHT]) return true;

    if(Movegen::PAWN_ATTACK_MOVES[!whoPlay][square] & enemies[Board::PAWN]) return true;

    if(Movegen::KING_MOVES[square] & enemies[Board::KING]) return true;
    return false;
}


void Board::makeNullMove() {
    State currentState{-1,enPassantSquare, castling, halfMove, zobristKey};
    push(false, currentState);

    zobristKey ^= Zobrist::sideToMove;
}

void Board::undoNullMove() {
    auto prevState = std::move(STACK[ply]);

    zobristKey = prevState.zobristHash;
    enPassantSquare = prevState.enPassantSquare;
    castling = prevState.castling;
    halfMove = prevState.halfMove;

    whoPlay = !whoPlay;

    repetitionIndex--;
    ply--;
}

std::string Board::FEN() const{
    std::stringstream ss;
    std::array<std::array<char, 8>,8> posArray;
    for (auto& row : posArray) {
        std::fill(row.begin(), row.end(), ' ');
    }

    for(int color = 0; color <= 1; color ++){
        for(int piece = 0; piece <= KING; piece++){
            auto white = color == 0;
            auto tmpBB = white ? whitePieces[piece] : blackPieces[piece];
            while(tmpBB){
                auto pos = bit_ops::bitScanForwardPopLsb(tmpBB);
                auto lIndex = pos / 8;
                auto rIndex = pos % 8;
                assert(rIndex < 8 && lIndex < 8);
                posArray[lIndex][rIndex] = white ? toupper(reversedPieceIndexMap[piece]) : reversedPieceIndexMap[piece];
            }
        }
    }
    for(int row = 0; row < 8; row++){
        auto emptyColumns = 0;
        for(int column = 0; column < 8; column++){
            if(posArray[row][column] == ' ') {
                emptyColumns++;
                continue;
            }
            if(emptyColumns != 0){
                ss << (char)('0' + emptyColumns);
                emptyColumns = 0;
            }
            ss << posArray[row][column];
        }
        if(emptyColumns != 0) ss << (char)('0' + emptyColumns);
        if(row != 7) ss << "/";
    }

    ss << " " << (whoPlay ? "w" : "b") << " ";

    auto any = castling[WHITE][K_CASTLE] || castling[WHITE][Q_CASTLE] || castling[BLACK][K_CASTLE] || castling[BLACK][Q_CASTLE];
    if(castling[WHITE][K_CASTLE]) ss << "K";
    if(castling[WHITE][Q_CASTLE]) ss << "Q";
    if(castling[BLACK][K_CASTLE]) ss << "k";
    if(castling[BLACK][Q_CASTLE]) ss << "q";

    if(!any) ss << "-";
    ss << " ";

    // en-passant square.
    if(enPassantSquare != -1){
        auto lIndex = enPassantSquare / 8;
        auto rIndex = enPassantSquare % 8;
        ss << (char)('a' + rIndex) << (char)('8' - lIndex);
    }
    else ss << "-";

    ss << " ";
    ss << halfMove << " ";
    // halfMove
    ss << fullMove;

    auto result = ss.str();
    return result;
}