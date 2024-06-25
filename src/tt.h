#ifndef SENTINEL_TT_H
#define SENTINEL_TT_H

#include <cstdint>
#include <move.h>
#include <board.h>

// https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm
class TranspositionTable {

public:
    enum HashType{
        EXACT,
        ALPHA,
        BETA
    };
    static constexpr int LOOKUP_ERROR = -1000000000;

    TranspositionTable() = default;

    TranspositionTable(int sizeMB, Board& board): _board(&board){
        uint64_t entrySize = sizeof(Entry);
        uint64_t sizeInBytes = sizeMB * 1024 * 1024;
        uint64_t numberOfEntries = sizeInBytes / entrySize;

        _count = numberOfEntries;
        _entries = new Entry[numberOfEntries];
    }

    void clear(){
        for(size_t j = 0; j < _count; j++){
            _entries[j].depth = -1;
        }
    }

    int getEval(int depth, int alpha, int beta){
        Entry entry = _entries[index()];
        if(entry.hash != _board->zobristKey) return LOOKUP_ERROR;

        if(entry.depth >= depth){
            if(entry.flag == HashType::EXACT){
                return entry.eval;
            }

            if(entry.flag == HashType::ALPHA && entry.eval <= alpha){
                return entry.eval;
            }

            if(entry.flag == HashType::BETA && entry.eval >= beta){
                return entry.eval;
            }
        }

        return LOOKUP_ERROR;
    }

    void store(int eval, int depth, uint64_t hash, HashType type, const Move& move){
        _entries[index()] = {eval, depth, hash, type, move};
    }

    Move getMove(){
        return _entries[index()].best;
    }

    void free(){
        delete[] _entries;
    }
private:
    int index(){
        return (int)(_board->zobristKey % _count);
    }


    struct Entry{
        int eval;
        int depth;
        uint64_t hash;
        HashType flag;
        Move best;
        Entry(int _eval, int _depth, uint64_t _hash, HashType _flag, Move _best)
                : eval(_eval), depth(_depth), hash(_hash), flag(_flag), best(_best) {};
        Entry() = default;
    };

    Entry* _entries;
    uint64_t _count;
    Board* _board;
};


#endif //SENTINEL_TT_H
