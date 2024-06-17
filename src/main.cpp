#include <tests/boardTests.cpp>
#include <magics.h>
#include <movegen.h>
#include <tests/perftTests.cpp>

#include <chrono>

int main(){
    Movegen::initTables();
    boardTests::testBoard();



    auto start = std::chrono::high_resolution_clock::now();
    perftTests::runTests();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // Convert duration to microseconds for better readability
    double microseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    std::cout << "Elapsed time: " << microseconds << " ms" << std::endl;



    return 6;

    Board board;
    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    board.printBoard();

    // 10'000'000 * 33 => 330 000 000 moves / ~10 sec => ~~ 30'000'000 moves/sec
    Move moves[Movegen::MAX_LEGAL_MOVES];
    std::cout <<Movegen::generateMoves(board, moves) << std::endl;

    return 6;
    int tc = 10'000'000;
    while(tc--){
        Movegen::generateMoves(board, moves);
    }


    return 0;
}