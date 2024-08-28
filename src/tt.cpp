#include <tt.h>
#include <cmath>
#include "search.h"

static constexpr int WIN_BOUND = Search::CHECKMATE - Search::MAX_DEPTH;

TranspositionTable::TranspositionTable(int sizeMB, Board& board): _board(&board){
    uint64_t entrySize = sizeof(Entry);
    uint64_t sizeInBytes = sizeMB * 1024 * 1024;
    uint64_t numberOfEntries = sizeInBytes / entrySize;

    _count = numberOfEntries;
    _entries = new Entry[numberOfEntries];
    std::cout << numberOfEntries << std::endl;
    std::cout << sizeof(Entry)<< std::endl;
}

int getCorrectedScore(int score, int ply){
    auto absScore = std::abs(score);
    if(absScore >= WIN_BOUND){
        score -= ply * (absScore/score);
    }
    return score;
}

int TranspositionTable::getEval(int depth, int alpha, int beta, int ply) {
    Entry entry = _entries[index()];
    if(entry.hash != _board->zobristKey) return LOOKUP_ERROR;

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


void TranspositionTable::store(int eval, int depth, TranspositionTable::HashType type, const Move &move, int ply) {
    auto key = index();

    if (eval >= WIN_BOUND) eval += ply;
    else if (eval <= -WIN_BOUND) eval -= ply;

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