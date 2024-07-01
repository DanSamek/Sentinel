#include <tt.h>

TranspositionTable::TranspositionTable(int sizeMB, Board& board): _board(&board){
    uint64_t entrySize = sizeof(Entry);
    uint64_t sizeInBytes = sizeMB * 1024 * 1024;
    uint64_t numberOfEntries = sizeInBytes / entrySize;

    _count = numberOfEntries;
    _entries = new Entry[numberOfEntries];
}

void TranspositionTable::clear() {
    for(size_t j = 0; j < _count; j++){
        _entries[j].depth = -1;
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
    _entries[index()] = {eval, depth, _board->zobristKey, type, move};
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