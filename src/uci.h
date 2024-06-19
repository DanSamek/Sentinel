#ifndef SENTINEL_UCI_H
#define SENTINEL_UCI_H
#include <iostream>
#include <movegen.h>
#include <sstream>

/***
 * Simple implementation of UCI protocol for a chess guis (only needed co)
 */
struct UCI {
    static inline Board board;
    static inline std::string startPos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    /***
     * Prints info about an engine
     */
    static void uciInit();

    /***
     * If engine is ready <-> if we have prepared tables etc.
     */
    static void isReady();

    /***
     * Init of a new game from starting position
     */
    static void newGame();

    /*** Format: 'position startpos moves e2e4 e7e5'
     * Or: 'position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 moves e2e4 e7e5'
     * Note: 'moves' section is optional
     */
    static void position(std::string command);

    /***
     * movetime 10
     * wtime 1000 btime 1000 winc 10 binc 10 TODO!
     * times are for searching.
     * @param command
     */
    static void go(std::string command);

    /***
     * Only for debug, if position is equal to a actual position in a GUI.
     */
    static void printPos();
private:
    static std::vector<std::string> parseMoves(std::string command);
    static void makeStringMove(std::string move);
};


#endif //SENTINEL_UCI_H
