#include <uci.h>
#include <search.h>
#include <zobrist.h>
#include <pst.h>

void UCI::uciInit() {
    std::cout << "id name Sentinel-PAWN-ISO80FX" << std::endl;
    std::cout << "id author Daniel Samek" << std::endl;
    std::cout << "uciok" << std::endl;
}

void UCI::isReady() {
    if(!_ready){
        Board::initPawnEvalBBS();
        Movegen::init();
        Zobrist::init();
        PST::init();
        _TT = TranspositionTable(_hashSize, _board);
        Search::TT = &_TT;
    }
    std::cout << "readyok" << std::endl;
    _ready = true;
}

void UCI::newGame() {
    _board.loadFEN(START_POS);
}

void UCI::position(std::string command) {
    if(command.find("startpos") != std::string::npos ){
        _board.loadFEN(START_POS);
    }
    else if(command.find("fen") != std::string::npos){
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
        _board.loadFEN(fen);
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

    /*
         movetime 10 mseconds - exactly.
         wtime 1000 btime 1000 winc 10 binc 10
     */
    std::istringstream iss(command);
    int miliseconds; std::string tmp;
    bool exact = false;
    if(command.find("movetime") != std::string::npos){
        iss >> tmp >> tmp >> miliseconds;
        exact = true;
    }
    else if(command.find("wtime") != std::string::npos){
        if(_board.whoPlay){
            iss >> tmp >> tmp >> miliseconds;
        }else{
            iss >> tmp >> tmp >> miliseconds >> tmp >> miliseconds;
        }
    }
    else{
        // inf search.
        miliseconds = INT_MAX;
    }

    auto move = Search::search(miliseconds, _board, exact);
    std::cout << "bestmove ";
    move.print();

    _board.makeMove(move);
    _board.printBoard();
}


void UCI::printPos() {
    _board.printBoard();
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
    auto result = Movegen::generateMoves(_board, moves);
    bool played = false;
    for(int j = 0; j < result.first; j++){
        if(moves[j].promotionType == t && moves[j].fromSq == fromSquare && moves[j].toSq == toSquare){
            _board.makeMove(moves[j]);
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

UCI::~UCI(){
    _TT.free();
}