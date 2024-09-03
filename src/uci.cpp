#include <uci.h>
#include <search.h>
#include <zobrist.h>
#include <pst.h>

void UCI::loop() {
    srand(time(nullptr));
    std::string command;
    while(getline(std::cin, command)){
        size_t commandEnd = command.find(" ");
        std::string commandName = command.substr(0, commandEnd);

        if(commandName == "uci"){
            UCI::uciInit();
        }
        else if(commandName == "isready"){
            UCI::isReady();
        }
        else if(commandName == "quit"){
            break;
        }
        else if(commandName == "ucinewgame"){
            UCI::newGame();
        }
        else if(commandName == "position"){
            UCI::position(command);
        }
        else if(commandName == "go"){
            UCI::go(command);
        }
        else if(commandName == "printPos"){
            UCI::printPos();
        }
        else if(commandName == "setoption"){
            UCI::setOption(command);
        }
    }
}

void UCI::uciInit() {
    std::cout << "id name Sentinel-CE" << std::endl;
    std::cout << "id author Daniel Samek" << std::endl << std::endl;
    std::cout << "option name Hash type spin default "<< _hashSize << " min 1 max 30000" << std::endl;
    std::cout << "uciok" << std::endl;
}

void UCI::isReady() {
    if(!_ready){
        Board::initPawnEvalBBS();
        Movegen::init();
        Zobrist::init();
        PST::init();
        reallocHashTable();
        _board.loadFEN(START_POS); // No more segfaults.
    }
    std::cout << "readyok" << std::endl;
    _ready = true;
}

void UCI::newGame() {
    _board.loadFEN(START_POS);
    if(_ready){
        _TT.free();
        _TT = TranspositionTable(_hashSize, _board);
        Search::TT = &_TT;
    }
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
        }
        else{
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
    if(!_ready){
        std::cout << "command isready wasnt called. Run first isready before go." << std::endl;
        return;
    }
    /*
        movetime 10 mseconds - exactly.
        wtime 1000 btime 1000 winc 10 binc 10
    */

    std::istringstream iss(command);
    int timeRemaining = 0; std::string tmp;
    int increment = 0;
    bool exact = false;
    if(command.find("movetime") != std::string::npos){
        iss >> tmp >> tmp >> timeRemaining;
        exact = true;
    }
    else if(command.find("wtime") != std::string::npos){
        // fuck it, we ball.
        if(_board.whoPlay){
            iss >> tmp >> tmp >> timeRemaining >> tmp >> increment >> tmp >> increment;
        }
        else{
            iss >> tmp >> tmp >> timeRemaining >> tmp >> timeRemaining >> tmp >> increment >> tmp >> increment;
        }
    }
    else{
        // inf search
        // TODO thread for a search -> stop command.
        timeRemaining = INT_MAX;
    }

    auto move = Search::search(timeRemaining, increment, _board, exact);
    std::cout << "bestmove ";
    move.print();

    _board.makeMove(move);
    _board.printBoard();
}


void UCI::setOption(std::string command) {
    auto stream = std::stringstream(command);
    std::string type, value;
    stream >> type >> type >> type >> value >> value;

    if(type == "Hash"){
        _hashSize = std::stoi(value);
        reallocHashTable();
    }
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
    auto result = Movegen(_board, moves, false).generateMoves();
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
    if(_ready) _TT.free();
}

void UCI::reallocHashTable(){
    if(_ready) _TT.free();
    _TT = TranspositionTable(_hashSize, _board);
    Search::TT = &_TT;
}