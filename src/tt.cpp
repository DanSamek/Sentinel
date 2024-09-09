#include <tt.h>
#include <cmath>
#include "search.h"


TranspositionTable::TranspositionTable(int sizeMB){
    uint64_t entrySize = sizeof(Entry);
    uint64_t sizeInBytes = sizeMB * 1024 * 1024;
    uint64_t numberOfEntries = sizeInBytes / entrySize;

    _count = numberOfEntries;
    entries = new Entry[numberOfEntries];
    std::cout << numberOfEntries << std::endl;
    std::cout << sizeof(Entry)<< std::endl;
}

int TranspositionTable::getCorrectedScore(int score, int ply){
    auto absScore = std::abs(score);
    if(absScore >= WIN_BOUND){
        score -= ply * (absScore/score);
    }
    return score;
}

void TranspositionTable::free() {
    delete[] entries;
}
