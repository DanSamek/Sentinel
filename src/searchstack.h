#ifndef SENTINEL_SEARCHSTACK_H
#define SENTINEL_SEARCHSTACK_H

#include <climits>
#include "move.h"
#include "consts.h"

struct Info{
    Move move;
    int score;
    Move excludedMove;
};

struct SearchStack{
    uint64_t nodesVisited = 0;
    uint64_t ttUsed = 0;

    int bestScore = INT_MIN;
    Move bestMove = {};

    Move pvTable[MAX_DEPTH][MAX_DEPTH];
    int pvLength[MAX_DEPTH];

    Info data[MAX_DEPTH];

    Info& operator[](int ply){
        return data[ply];
    }
};

#endif //SENTINEL_SEARCHSTACK_H
