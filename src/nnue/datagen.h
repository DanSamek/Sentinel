#ifndef SENTINEL_DATAGEN_H
#define SENTINEL_DATAGEN_H

#include <atomic>
#include "timer.h"

class Datagen {
public:
    Datagen(int threadCount, int softNodeLimit = 5000, int maximumPositions = 1000) : _threadCnt(threadCount), _softNodeLimit(softNodeLimit), _maximumPositions(maximumPositions){};
    void run();
private:
    void runWorker(int softNodeLimit, int threadId);
    int _threadCnt;
    int _softNodeLimit;
    std::atomic<bool> _stopSignal;
    Timer _timer;
    std::atomic<int> _gamesPlayed = 0;
    std::atomic<int> _totalPos = 0;
    int _maximumPositions = 0;
    std::mutex _printMutex;

    struct Position{
        std::string fen;
        int score; // white relative.
    };
};

#endif //SENTINEL_DATAGEN_H
