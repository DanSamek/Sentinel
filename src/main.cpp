#include <tests/boardTests.cpp>
#include <magics.h>
#include <movegen.h>
#include <tests/perftTests.cpp>

#include <chrono>
#include "uci.h"

#define RUN_TESTS false

int main(){

    if(RUN_TESTS){
        Movegen::initTables();
        boardTests::testBoard();
        auto start = std::chrono::high_resolution_clock::now();
        perftTests::runTests(true);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        // Convert duration to microseconds for better readability
        double microseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        std::cout << "Elapsed time: " << microseconds << " ms" << std::endl;



        start = std::chrono::high_resolution_clock::now();
        perftTests::runTests(false);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;

        // Convert duration to microseconds for better readability
        microseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        std::cout << "Elapsed time: " << microseconds << " ms" << std::endl;

    }else{
        std::string command;
        while((std::cin >> command)){
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

        }
    }




    return 0;
}