#ifndef SENTINEL_TT_H
#define SENTINEL_TT_H

#include <cstdint>
#include <move.h>
#include <board.h>

// https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm
class TranspositionTable {
public:
    enum HashType : char{
        EXACT,
        UPPER_BOUND,
        LOWER_BOUND
    };
    static constexpr int LOOKUP_ERROR = -1000000000;
    static constexpr int FOUND_NOT_ACCEPTED = -2000000000;

    // Default .ctor for uci init (not used)
    TranspositionTable() = default;

    /***
     * @param sizeMB size of TT in MB (64,128,256,..) even custom.
     * @param board for getting current zobrist key.
     */
    TranspositionTable(int sizeMB, Board& board);

    /***
     * TT clear - TODO fix, slow everything down
     * All calculated stuff can be used.
     * Propably problems with repetitions in position, we need to somehow add to zobrist key even cnt of repetitions or idk ?!?
     */
    void clear();

    /***
     * @param depth current depth in search.
     * @param alpha
     * @param beta
     * @return a value from TT, if not found, returns @see TranspositionTable::LOOKUP_ERROR.
     */
    int getEval(int depth, int alpha, int beta);

    /***
     * Stores a current position to a TT.
     * @param eval @see Board::eval().
     * @param depth current depth in search.
     * @param type "hash type".
     * @param move best move for this position.
     */
    void store(int eval, int depth, HashType type, const Move& move);

    /***
     * @return best move for a current position.
     */
    Move getMove();

    /***
     * @return current flag @see HashType
     */
    HashType getHashFlag();

    /***
     * Frees an allocated memory for a table.
     */
    void free();
private:

    int index();
    static inline Move NO_MOVE = Move();

    struct Entry{
        int eval;
        int8_t depth;
        uint64_t hash;
        HashType flag;
        Move best;
        Entry(int _eval, int _depth, uint64_t _hash, HashType _flag, Move _best)
                : eval(_eval), depth(_depth), hash(_hash), flag(_flag), best(_best) {};
        Entry(){
            best = NO_MOVE;
            depth = -1;
            hash = 0ULL;
        }
    };

    Entry* _entries;
    uint64_t _count;
    Board* _board;
};


#endif //SENTINEL_TT_H
