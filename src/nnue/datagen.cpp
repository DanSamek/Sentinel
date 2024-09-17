#include "datagen.h"
#include "thread"
#include "vector"
#include "iostream"
#include "board.h"
#include "movegen.h"
#include "zobrist.h"
#include "search.h"

static inline const std::string START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void Datagen::run() {
    // Engine stuff - for a search.
    Board::initPawnEvalBBS();
    Movegen::init();
    Zobrist::init();
    PST::init();

    srand(time(nullptr));

    _timer = Timer(0, true);
    // worker threads.
    std::vector<std::thread> workers;
    for(int i = 0; i < _threadCnt; i++){
        workers.emplace_back([this, i](){
            runWorker(_softNodeLimit, i);
        });
    }

    // waiting for a user message.
    std::cout << "waiting on `stop` message" << std::endl;
    std::string userMessage;
    while(true){
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
    std::cout << "worker - " << threadId << " is running." << std::endl;
    auto board = Board();
    std::ostringstream oss;

    auto fileName = oss.str();
    const Move NO_MOVE = Move();

    Search s = Search();
    while(!_stopSignal){
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
            board.makeMove(move);

            auto isCheck = board.inCheck();
            if(!move.isCapture() && move.moveType != Move::EN_PASSANT && !isCheck){
                totalPos++;
                auto whiteRelativeScore = !board.whoPlay ? score : -score;
                positions.push_back({board.FEN(), whiteRelativeScore, winner});
            }

            auto absScore = std::abs(score);
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
                winner = score > 0 ? !board.whoPlay ? 1 : 0 : board.whoPlay ? 0 : 1;
                break;
            }

            if(draw_counter > 8){
                winner = 0.5;
                break;
            }

            // Checkmate
            if(NO_MOVE == move && std::abs(score) >= (1000000 - 1000)){
                winner = score > 0 ? !board.whoPlay ? 1 : 0 : board.whoPlay ? 0 : 1;
                break;
            }
            // Draw [stalemate,..]
            else if(NO_MOVE == move){
                winner = 0.5;
                break;
            }

            if(_stopSignal) break;
        }
        TT.free();
        _gamesPlayed++;
        _totalPos += totalPos;

        // TODO save positions to a file.
        for(const auto& position : positions){
            // write to a file.
        }
        if(threadId == 0){
            std::cout << " games played: " << _gamesPlayed  << " pos/s " << (_totalPos) / (_timer.getMs() / 1000.0) << " total pos: " << _totalPos << " total " << _timer.getMs() << std::endl;
            std::cout.flush();
        }
    }
}