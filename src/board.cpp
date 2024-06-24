#include <sstream>
#include <board.h>
#include <zobrist.h>
#include <movegen.h>

/*Macros for simplier code, don't want to create some functions to make it slower*/
#define MOVE_PIECE(currentPieces, movePiece, fromSq, toSq) \
    bit_ops::setNthBit(currentPieces[movePiece], toSq); \
    bit_ops::popNthBit(currentPieces[movePiece], fromSq)


#define HANDLE_CASTLING(move, whoPlay, currentCastling) \
    switch (move.movePiece) { \
        case ROOK: \
            if (whoPlay) { \
                switch (move.fromSq) { \
                    case 56: \
                        currentCastling[Q_CASTLE] = false; \
                        break; \
                    case 63: \
                        currentCastling[K_CASTLE] = false; \
                        break; \
                    default: \
                        break; \
                } \
            } else { \
                switch (move.fromSq) { \
                    case 0: \
                        currentCastling[Q_CASTLE] = false; \
                        break; \
                    case 7: \
                        currentCastling[K_CASTLE] = false; \
                        break; \
                    default: \
                        break; \
                } \
            } \
            break; \
        case KING: \
            currentCastling[0] = false; \
            currentCastling[1] = false; \
            break; \
        default: \
            break; \
    }

#define HANDLE_ENEMY_CASTLING(type, move, whoPlay, enemyCastling) \
    switch (type.first) { \
        case ROOK: \
            if (whoPlay) { \
                switch (move.toSq) { \
                    case 0: \
                        enemyCastling[Q_CASTLE] = false; \
                        break; \
                    case 7: \
                        enemyCastling[K_CASTLE] = false; \
                        break; \
                    default: \
                        break; \
                } \
            } else { \
                switch (move.toSq) { \
                    case 56: \
                        enemyCastling[Q_CASTLE] = false; \
                        break; \
                    case 63: \
                        enemyCastling[K_CASTLE] = false; \
                        break; \
                    default: \
                        break; \
                } \
            } \
            break; \
        default: \
            break; \
    }


int Board::eval() {
    int whiteScore = evalSide(whitePieces);
    int blackScore = evalSide(blackPieces);
    return (whiteScore - blackScore) * (whoPlay ? 1 : -1);
}


int Board::evalSide(uint64_t *bbs) const{
    int eval = 0;
    for(int j = 0; j < 6; j++){
        auto bb = bbs[j];
        int cnt = 0;
        while(bb){
            cnt++;
            bit_ops::bitScanForwardPopLsb(bb);
        }
        eval += cnt * PIECE_EVAL_EARLY[j];
    }
    return eval;
}


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
    fiftyMoveRule[0] = 0; fiftyMoveRule[1] = 0;
    repetitionIndex = 0;

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

    State currentState{-1,enPassantSquare, castling, fiftyMoveRule, zobristKey};

    std::pair<uint64_t , bool> type;
    switch (move.moveType) {
        case Move::CAPTURE:
            // simple move.
            MOVE_PIECE(currentPieces, move.movePiece, move.fromSq, move.toSq);

            // find enemy piece and pop bit.
            type = getPieceTypeFromSQ(move.toSq, enemyPieces);
            bit_ops::popNthBit(enemyPieces[type.first], move.toSq);

            currentState.captureType = type.first;
            // !! castling !! <-> for enemy !!

            HANDLE_ENEMY_CASTLING(type, move, whoPlay, enemyCastling);
            HANDLE_CASTLING(move, whoPlay, currentCastling);
            fiftyMoveRule[whoPlay] = 0;
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
                HANDLE_ENEMY_CASTLING(type, move, whoPlay, enemyCastling);
            }
            fiftyMoveRule[whoPlay] = 0;
            break;
        case Move::QUIET:
            // !!! rooks  !!! <-> disable castling.
            MOVE_PIECE(currentPieces, move.movePiece, move.fromSq, move.toSq);
            HANDLE_CASTLING(move, whoPlay, currentCastling);

            if(move.movePiece == Board::PAWN) fiftyMoveRule[whoPlay] = 0;
            else fiftyMoveRule[whoPlay]++;
            break;
        case Move::EN_PASSANT:
            MOVE_PIECE(currentPieces, move.movePiece, move.fromSq, move.toSq);
            bit_ops::popNthBit(enemyPieces[move.movePiece], whoPlay ? move.toSq + 8 : move.toSq - 8);

            fiftyMoveRule[whoPlay] = 0;
            break;
        case Move::CASTLING:
            // kingSide
            MOVE_PIECE(currentPieces, KING, move.fromSq, move.toSq);
            if(move.fromSq < move.toSq){
                MOVE_PIECE(currentPieces, ROOK, move.toSq + 1, move.toSq - 1);
            }
            // queenSide
            else{
                MOVE_PIECE(currentPieces, ROOK,  move.toSq - 2, move.toSq + 1);
            }
            currentCastling[0] = false;
            currentCastling[1] = false;

            fiftyMoveRule[whoPlay]++;
            break;
        case Move::DOUBLE_PAWN_UP:
            MOVE_PIECE(currentPieces, move.movePiece, move.fromSq, move.toSq);
            enPassantSquare = (move.fromSq + move.toSq) / 2;
            setEnPassant = true;

            fiftyMoveRule[whoPlay] = 0;
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

    // update and add to a move "stack".
    // save state to a current depth
    switch (move.moveType) {
        case Move::CAPTURE:
            Zobrist::updateHashMove(zobristKey, move,*this, currentState);
            break;
        case Move::PROMOTION:
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
    halfMove++;
    repetitionIndex++;
    STACK[halfMove] = std::move(currentState);

    whoPlay = !whoPlay;
    enPassantSquare = setEnPassant ? enPassantSquare : -1;
}

void Board::undoMove(const Move &move) {
    auto prevState = std::move(STACK[halfMove]);
    whoPlay = !whoPlay;
    repetitionIndex--;

    auto currentPieces = whoPlay ? whitePieces : blackPieces;
    auto enemyPieces = !whoPlay ? whitePieces : blackPieces;

    castling = std::move(prevState.castling); // set prev castling rules.
    enPassantSquare = prevState.enPassantSquare; // reset of an en passant square.

    zobristKey = prevState.zobristHash;
    fiftyMoveRule = prevState.fiftyMoveRule;

    switch (move.moveType) {
        case Move::CAPTURE:
            MOVE_PIECE(currentPieces, move.movePiece, move.toSq, move.fromSq);
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
            MOVE_PIECE(currentPieces, move.movePiece, move.toSq, move.fromSq);
            break;
        case Move::EN_PASSANT:
            // move back pawn, "respawn" enemyPawn.
            bit_ops::setNthBit(enemyPieces[move.movePiece], whoPlay ? move.toSq + 8 : move.toSq - 8);
            MOVE_PIECE(currentPieces, move.movePiece, move.toSq, move.fromSq);
            break;
        case Move::CASTLING:
            // move rook and king to original squares.
            // kingSide
            MOVE_PIECE(currentPieces, KING, move.toSq, move.fromSq);
            if(move.fromSq < move.toSq){
                MOVE_PIECE(currentPieces, ROOK, move.toSq - 1, move.toSq + 1);
            }
            // queenSide
            else{
                MOVE_PIECE(currentPieces, ROOK, move.toSq + 1,  move.toSq - 2);
            }
            break;
        case Move::DOUBLE_PAWN_UP:
            MOVE_PIECE(currentPieces, move.movePiece, move.toSq, move.fromSq);
            break;
    }
    halfMove--;
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


bool Board::isDraw(){
    if(isThreeFoldRepetition()) return true;

    if(fiftyMoveRule[0] >= 50 && fiftyMoveRule[1] >= 50) return true;

    // count material from bbs
    return isInsufficientMaterial(whitePieces) || isInsufficientMaterial(blackPieces);
}

bool Board::isThreeFoldRepetition() const {
    int cnt = 0;
    for(int j = 0; j < repetitionIndex; j++){
        if(threeFoldRepetition[j] == zobristKey) cnt++;
        if(cnt == 2) return true; // 1 me + 2 in history.
    }
    return false;
}


bool Board::isInsufficientMaterial(uint64_t* bbs) const{
    // pawns, rooks, queens.
    if(bbs[PAWN] || bbs[ROOK] || bbs[QUEEN]) return false;

    auto bb = bbs[BISHOP];
    int cnt = bit_ops::countBits(bb);
    if(cnt >= 2) return false;

    bb = bbs[KNIGHT];
    cnt = bit_ops::countBits(bb);
    if(cnt >= 2) return false;

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
    // validate checks.
    if(Magics::getRookMoves(all, square) & (enemies[Board::ROOK] | enemies[Board::QUEEN])) return true;

    if(Magics::getBishopMoves(all, square) & (enemies[Board::BISHOP] | enemies[Board::QUEEN])) return true;

    // knights.
    if(Movegen::KNIGHT_MOVES[square] & enemies[Board::KNIGHT]) return true;

    // pawns
    if(Movegen::PAWN_ATTACK_MOVES[!whoPlay][square] & enemies[Board::PAWN]) return true;

    // kings.
    if(Movegen::KING_MOVES[square] & enemies[Board::KING]) return true;
    return false;
}
