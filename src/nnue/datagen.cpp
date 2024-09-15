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

    // worker threads.
    std::vector<std::thread> workers;
    for(int i = 0; i < _threadCnt; i++){
        workers.emplace_back([this, i](){
            runWorker(_searchDepth, i);
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

// TODO, when search will be faster !
void Datagen::runWorker(int maxDepth, int threadId) {
    // init of search.
    auto board = Board();
    std::ostringstream oss;

    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    oss << "th-id" << threadId << "time" << duration.count();
    auto fileName = oss.str();
    auto gamesPlayed = 0;
    auto totalPos = 0;
    const Move NO_MOVE = Move();
    while(!_stopSignal){
        // position startup.
        again:
        board.loadFEN(START_POS);
        
        auto TT = TranspositionTable(16);
        Search s = Search();
        s.TT = &TT;
        
        // playout 8 or 9 random moves.
        int moveCnt = rand() % 50 ? 8 : 9;
        while(moveCnt--){
            Move moves[Movegen::MAX_LEGAL_MOVES];
            auto [moveCount, isCheck] = Movegen(board, moves).generateMoves<false>();
            auto randomMoveIndex = rand () % moveCount;
            auto cnt = 1;
            while(!board.makeMove(moves[randomMoveIndex])){
                randomMoveIndex = rand () % moveCount;
                if(isCheck && cnt >= 5){
                    TT.free();
                    goto again;
                }
                cnt++;
            }
        }

        auto [move, score] = s.datagen(board, maxDepth);

        // filter out pretty unbalanced positions.
        if(std::abs(score) >= 1000){
            std::lock_guard<std::mutex> lock(_coutMutex);
            std::cout << "skipping position";
            TT.free();
            continue;
        }

        // playout a game.
        while(true){
            auto [move, score] = s.datagen(board, maxDepth);
            board.makeMove(move);
            if(!move.isCapture() && move.moveType != Move::EN_PASSANT && !board.inCheck()){
                // TODO.
            }

            if(std::abs(score) >= 1000){
                // winning game
                break;
            }

            if(board.isDraw() || NO_MOVE == move){
                // TODO.
                break;
            }
            // if we are in check, generate moves, if its mate.

            if(board.inCheck()){
                Move moves[Movegen::MAX_LEGAL_MOVES];
                auto [moveCount, isCheck] = Movegen(board, moves).generateMoves<false>();
                bool madeAMove = false;
                for(int j = 0; j < moveCount; j++){
                    if(!board.makeMove(moves[j])) continue;
                    madeAMove = true;
                    board.undoMove(moves[j]);
                    break;
                }
                if(!madeAMove){
                    // checkmate.
                    break;
                }
            }

            totalPos++;
        }
        TT.free();
        gamesPlayed++;
        if(gamesPlayed % 10 == 0){
            std::lock_guard<std::mutex> lock(_coutMutex);
            std::cout << "thread-" << threadId << " games played: " << gamesPlayed  << " totalPositions: " << totalPos << std::endl;
            std::cout.flush();
        }
    }
}