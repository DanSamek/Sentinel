#include <tt.h>
#include <cmath>
#include "search.h"

TranspositionTable::TranspositionTable(int sizeMB, Board& board): _board(&board){
    uint64_t entrySize = sizeof(Entry);
    uint64_t sizeInBytes = sizeMB * 1024 * 1024;
    uint64_t numberOfEntries = sizeInBytes / entrySize;

    //uint64_t powerOfTwoEntries = 1ULL << static_cast<uint64_t>(std::log2(numberOfEntries));

    _count = numberOfEntries;
    _entries = new Entry[numberOfEntries];
    std::cout << numberOfEntries << std::endl;
    std::cout << sizeof(Entry)<< std::endl;
}

void TranspositionTable::clear() {
    for(size_t j = 0; j < _count; j++){
        _entries[j] = Entry();
    }
}

int TranspositionTable::getEval(int depth, int alpha, int beta) {
    Entry entry = _entries[index()];
    if(entry.hash != _board->zobristKey) return LOOKUP_ERROR;

    if(entry.depth >= depth){
        if(entry.flag == HashType::EXACT){
            return entry.eval;
        }

        if(entry.flag == HashType::UPPER_BOUND && entry.eval <= alpha){
            return entry.eval;
        }

        if(entry.flag == HashType::LOWER_BOUND && entry.eval >= beta){
            return entry.eval;
        }
    }

    return FOUND_NOT_ACCEPTED;
}

void TranspositionTable::store(int eval, int depth, TranspositionTable::HashType type, const Move &move) {
    auto key = index();

    //if(_board->zobristKey == _entries[key].hash && _entries[key].depth > depth) return;
    _entries[key] = {eval, depth, _board->zobristKey, type, move};
}

Move TranspositionTable::getMove() {
    return _entries[index()].best;
}

TranspositionTable::HashType TranspositionTable::getHashFlag() {
    return _entries[index()].flag;
}

void TranspositionTable::free() {
    delete[] _entries;
}

int TranspositionTable::index() {
    return (int)(_board->zobristKey % _count);
}