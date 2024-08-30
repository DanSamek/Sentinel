#ifndef SENTINEL_UCI_H
#define SENTINEL_UCI_H
#include <iostream>
#include <movegen.h>
#include <sstream>
#include <tt.h>

/***
 * Simple implementation of UCI for a chess GUIs (only needed commands).
 */
struct UCI {
    static inline Board _board;
    static inline TranspositionTable _TT;
    static inline int _hashSize = 128;
    static inline const std::string START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    static inline bool _ready = false;

    /***
     * Command line input loop for all commands.
     */
    static void loop();

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

    /***
     * Format: 'position startpos moves e2e4 e7e5'
     * Or: 'position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 moves e2e4 e7e5'
     * Note: 'moves' section is optional
     */
    static void position(std::string command);

    /***
     * movetime 10
     * wtime 1000 btime 1000 winc 10 binc 10
     */
    static void go(std::string command);

    /***
     * Only for debug, if position is equal to a actual position in a GUI.
     * -> Not un UCI (i hope :D).
     */
    static void printPos();

    /***
     * @param command - for example set option Hash value 32
     */
    static void setOption(std::string command);


    ~UCI();
private:
    static std::vector<std::string> parseMoves(std::string command);
    static void makeStringMove(std::string move);
    static void reallocHashTable();
};


#endif //SENTINEL_UCI_H
