#ifndef SENTINEL_TT_H
#define SENTINEL_TT_H

#include <cstdint>
#include <move.h>
#include <board.h>
#include <consts.h>

// https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm
class TranspositionTable {
public:
    enum HashType : char{
        EXACT,
        UPPER_BOUND,
        LOWER_BOUND
    };

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
            flag = UPPER_BOUND;
        }

        bool valid(){
            return hash != 0ULL;
        }
    };

    static inline const Entry NO_ENTRY = {};
    Entry* entries;

    static constexpr int LOOKUP_ERROR = -1000000000;
    static constexpr int FOUND_NOT_ACCEPTED = -2000000000;

    // Default .ctor for uci init (not used)
    TranspositionTable() = default;

    /***
     * @param sizeMB size of TT in MB (64,128,256,..) even custom.
     */
    TranspositionTable(int sizeMB);


    /***
     * @param zobristKey current key of a position.
     * @param depth current depth in search.
     * @param alpha
     * @param beta
     * @return a value from TT, if not found, returns @see TranspositionTable::LOOKUP_ERROR.
     */
    inline int getEval(const uint64_t& zobristKey, int index,int depth, int alpha, int beta, int ply) {
        const auto& entry = entries[index];
        if(entry.hash != zobristKey) return LOOKUP_ERROR;

        if(entry.depth >= depth){

            auto correctedEval = getCorrectedScore(entry.eval, ply);

            if(entry.flag == HashType::EXACT){
                return correctedEval;
            }

            if(entry.flag == HashType::UPPER_BOUND && entry.eval <= alpha){
                return correctedEval;
            }

            if(entry.flag == HashType::LOWER_BOUND && entry.eval >= beta){
                return correctedEval;
            }
        }

        return FOUND_NOT_ACCEPTED;
    }

    /***
     * Stores a current position to a TT.
     * @param zobristKey current key of a position.
     * @param eval @see Board::eval().
     * @param depth current depth in search.
     * @param type "hash type".
     * @param move best move for this position.
     */
    inline void store(const uint64_t& zobristKey, int index, int eval, int depth, TranspositionTable::HashType type, const Move &move, int ply) {
        if (eval >= WIN_BOUND) eval += ply;
        else if (eval <= -WIN_BOUND) eval -= ply;

        // if(entries[index].depth >= depth && zobristKey == entries[index].hash) return;
        entries[index] = {eval, depth, zobristKey, type, move};
    }

    const Entry* getEntry(int index){
        return &entries[index];
    }

    /***
     * Frees an allocated memory for a table.
     */
    void free();

    inline int index(const uint64_t& key) {
        return (int)(key % _count);
    }

    inline void prefetch(const uint64_t& key){
        __builtin_prefetch(&entries[key]);
    }

private:

    int getCorrectedScore(int score, int ply);

    uint64_t _count;
};


#endif //SENTINEL_TT_H
