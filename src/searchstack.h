#ifndef SENTINEL_SEARCHSTACK_H
#define SENTINEL_SEARCHSTACK_H

#include <climits>
#include "move.h"
#include "consts.h"

struct Info{
    Move move;
    int score;
};

struct SearchStack{
    int nodesVisited = 0;
    int ttUsed = 0;

    int bestScore = INT_MIN;
    Move bestMove = {};

    Move pvTable[MAX_DEPTH][MAX_DEPTH];
    int pvLength[MAX_DEPTH];

    Info data[MAX_DEPTH];
};

#endif //SENTINEL_SEARCHSTACK_H
