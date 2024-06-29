#include <movegen.h>
#include <tests/PerftTests.cpp>
#include <tests/BoardTests.cpp>
#include <tests/ZobristTests.cpp>
#include <tests/nullMoveTests.cpp>

#include <chrono>
#include <uci.h>

#define RUN_TESTS false

int main(){
    if(RUN_TESTS){
        Movegen::init();
        PST::init();
        Zobrist::init();
        BoardTests::testBoard();
        NullMoveTests::runTests();
        ZobristTests::runTests();
        auto start = std::chrono::high_resolution_clock::now();
        PerftTests::runTests();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        // Convert duration to microseconds for better readability
        double microseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        std::cout << "Elapsed time: " << microseconds << " ms" << std::endl;

    }else{
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

        }
    }




    return 0;
}