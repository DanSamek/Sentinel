#include "datagen.h"
#include "thread"
#include "vector"
#include "iostream"
#include "board.h"
#include "movegen.h"
#include "zobrist.h"
#include "search.h"
#include "fstream"
#include "iomanip"

static inline const std::string START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void printStartupText(){
    // https://www.patorjk.com/software/taag/#p=display&f=Graffiti&t=Sentinel
    std::cout << "  _________              __  .__              .__   " << std::endl;
    std::cout << " /   _____/ ____   _____/  |_|__| ____   ____ |  |  "<< std::endl;
    std::cout << " \\_____  \\_/ __ \\ /    \\   __\\  |/    \\_/ __ \\|  |  "<< std::endl;
    std::cout << " /        \\  ___/|   |  \\  | |  |   |  \\  ___/|  |__"<< std::endl;
    std::cout << "/_______  /\\___  >___|  /__| |__|___|  /\\___  >____/"<< std::endl;
    std::cout << "        \\/     \\/     \\/             \\/     \\/      "<< std::endl;
    std::cout << "Datagen tool" << std::endl;
}

void Datagen::run() {
    // Engine stuff - for a search.
    Board::initPawnEvalBBS();
    Movegen::init();
    Zobrist::init();
    PST::init();
    printStartupText();

    srand(time(nullptr));

    _timer = Timer(0, true);
    // worker threads.
    std::vector<std::thread> workers;
    for(int i = 0; i < _threadCnt; i++){
        std::cout << "worker - " << i << " is running." << std::endl;
        workers.emplace_back([this, i](){
            runWorker(_softNodeLimit, i);
        });
    }

    // waiting for a user message.
    std::cout << "waiting on `stop` message" << std::endl;
    std::string userMessage;
    while(_totalPos < _maximumPositions){
        // handle stop signal
        std::cin >> userMessage;
        if(userMessage == "stop"){
            _stopSignal = true;
            break;
        }
    }

    for(auto& worker : workers){
        if(worker.joinable()){
            worker.join();
        }
    }
}


void Datagen::runWorker(int softNodeLimit, int threadId) {
    // init of search.
    auto board = Board();
    std::ostringstream oss;

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&currentTime);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", localTime);

    oss << "th-" << threadId << buffer << ".txt";
    auto fileName = oss.str();
    std::ofstream file(fileName);
    file << std::fixed << std::setprecision(1);

    const Move NO_MOVE = Move();

    Search s = Search();
    while(!_stopSignal && _totalPos < _maximumPositions){
        // Position startup.
        again:
        board.loadFEN(START_POS);
        
        // Play out 8 or 9 random moves.
        int moveCnt = rand() % 50 ? 8 : 9;
        while(moveCnt--){
            Move moves[Movegen::MAX_LEGAL_MOVES];
            auto [moveCount, isCheck] = Movegen(board, moves).generateMoves<false>();
            auto randomMoveIndex = rand () % moveCount;
            auto cnt = 1;
            while(!board.makeMove(moves[randomMoveIndex])){
                randomMoveIndex = rand () % moveCount;
                if(isCheck && cnt >= 3){
                    goto again;
                }
                cnt++;
            }
        }

        auto TT = TranspositionTable(16);
        s.TT = &TT;

        auto [move, score] = s.datagen(board, 11);

        // Filter out pretty unbalanced positions.
        if(std::abs(score) >= 1000){
            TT.free();
            continue;
        }

        // Play out a game.
        int totalPos = 0;
        double winner = 0; // Draw = 0.5, white = 1, black = 0;

        int win_counter = 0;
        int draw_counter = 0;

        std::vector<Position> positions;

        while(true){
            auto [move, score] = s.datagen(board, softNodeLimit);
            // Draw [stalemate,..]
            if(NO_MOVE == move){
                winner = 0.5;
                break;
            }
            auto absScore = std::abs(score);
            auto isMate = absScore >= (1'000'000 - 1000);


            auto isCheck = board.inCheck();
            auto whiteRelativeScore = board.whoPlay ? score : -score;
            if(!move.isCapture() && move.moveType != Move::EN_PASSANT && !isCheck){
                if(isMate && whiteRelativeScore > 0) whiteRelativeScore -= 970'000; // int16 range.
                if(isMate && whiteRelativeScore < 0) whiteRelativeScore += 970'000; // int16 range.

                totalPos++;
                positions.push_back({board.FEN(), whiteRelativeScore});
            }


            if(absScore >= 2000){
                win_counter++;
                draw_counter = 0;
            }
            else if(absScore <= 5){
                draw_counter++;
                win_counter = 0;
            }
            else{
                win_counter = 0;
                draw_counter = 0;
            }

            if(win_counter > 5){
                winner = whiteRelativeScore > 0;
                break;
            }

            if(draw_counter > 7){
                winner = 0.5;
                break;
            }

            // Checkmate
            if(isMate){
                winner = whiteRelativeScore > 0;
                break;
            }

            // make a move.
            board.makeMove(move);

            if(_stopSignal) break;
        }
        TT.free();
        _gamesPlayed++;
        _totalPos += totalPos;
        if(!_stopSignal) {
            for (const auto &position: positions) {
                // write to a file.
                file << position.fen << " | " << position.score << " | " << winner << std::endl;
            }
        }

        if(threadId == 0 && (_totalPos % 10'000) <= 500){
            printInfo();
        }
    }
    // close a stream
    file.close();

    std::lock_guard<std::mutex> guard(_printMutex);
    std::cout << "worker - " << threadId << " ended." << std::endl;
}

void Datagen::printInfo() {
    double percent = (_totalPos / (_maximumPositions * 1.0));
    const int barWidth = 50;
    int position = barWidth * percent;
    auto posPerSecond = (_totalPos) / (_timer.getMs() / 1000.0);
    auto diff = _maximumPositions - _totalPos;
    std::cout << "----------------" << std::endl;
    std::cout << "total games played: " << _gamesPlayed  << std::endl;
    std::cout << "positions/s: " <<  posPerSecond << std::endl;
    std::cout << "total generated positions [quiet]: " << _totalPos << std::endl;
    std::cout << "total time [seconds]: " << (_timer.getMs()/1000) << std::endl;
    std::cout << "time remaining [seconds]: " <<  diff / posPerSecond   << std::endl;

    std::cout << "Progress: " << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < position) std::cout << "=";
        else if (i == position) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(percent * 100.0) << " %";
    std::cout << std::endl << std::endl;

    std::cout.flush();
}
