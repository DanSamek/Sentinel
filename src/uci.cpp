#include "uci.h"

void UCI::uciInit() {
    std::cout << "id name Sentinel" << std::endl;
    std::cout << "id author Daniel Samek" << std::endl;
    std::cout << "uciok" << std::endl;
}


void UCI::isReady() {
    Movegen::initTables(); // prop do sentinel::init();
    std::cout << "readyok" << std::endl;
}

void UCI::newGame() {
    board.loadFEN(startPos);
}

void UCI::position(std::string command) {
    std::transform(command.begin(), command.end(), command.begin(), [](unsigned char c) { return std::tolower(c); });

    if(command.find("startpos")){
        board.loadFEN(startPos);
    }
    else if(command.find("fen")){
        size_t posFEN = command.find("fen");
        size_t posMoves = command.find("moves");

        size_t start = posFEN + 4;
        std::string fen;
        if (posMoves != std::string::npos) {
            size_t length = posMoves - start;
            fen = command.substr(start, length);
        } else{
            fen = command.substr(start);
        }
        board.loadFEN(fen);
    }
    else{
        std::cout << "Invalid position command (startpos || fen expected)" << std::endl;
        return;
    }
    // moves.
    std::vector<std::string> moves = parseMoves(command);

    // no moves, ok.
    if(moves.size() == 0) return;
    // play all moves.

    for(auto move: moves){
        makeStringMove(move);
    }
}

void UCI::go(std::string command) {
    Move moves[Movegen::MAX_LEGAL_MOVES];
    int size = Movegen::generateMoves(board, moves);
    // print move, now random for test.
    auto randomMove = moves[rand() % size];
    std::cout << "bestmove ";
    randomMove.print();
}


void UCI::printPos() {
    board.printBoard();
}


void UCI::makeStringMove(std::string move) {
    int fromSquare = (move[0] - 'a') + (7 - (move[1] - '1')) * 8;
    int toSquare = (move[2] - 'a') + (7 - (move[3] - '1')) * 8;

    Move::PromotionType t = Move::NONE;
    if (move.length() == 5) {
        switch (move[4]) {
            case 'q':
                t = Move::QUEEN;
                break;
            case 'n':
                t = Move::KNIGHT;
                break;
            case 'b':
                t = Move::BISHOP;
                break;
            case 'r':
                t = Move::ROOK;
                break;
            default:
                break;
        }
    }
    // make move
    Move moves[Movegen::MAX_LEGAL_MOVES];
    int size = Movegen::generateMoves(board, moves);
    bool played = false;
    for(int j = 0; j < size; j++){
        if(moves[j].promotionType == t && moves[j].fromSq == fromSquare && moves[j].toSq == toSquare){
            board.makeMove(moves[j], board.halfMove);
            played = true;
            break;
        }
    }
    if(!played) std::cout << "move not found" << std::endl;
}



std::vector<std::string> UCI::parseMoves(std::string command) {
    size_t posMoves = command.find("moves");
    if(posMoves == std::string::npos) return {};
    std::string movesSubstr = command.substr(posMoves + 6);
    std::stringstream ss(movesSubstr);
    std::string move;
    std::vector<std::string> moves;
    while((ss >> move)){
        moves.push_back(move);
    }
    return moves;
}