#ifndef SENTINEL_DATAGEN_H
#define SENTINEL_DATAGEN_H

#include <atomic>
#include <timer.h>
#include <mutex>

class Datagen {
public:
    Datagen();
    void run();
private:
    void printStartupText();
    void runWorker(int softNodeLimit, int threadId);
    void printInfo();
    int _threadCnt;
    int _softNodeLimit;
    int _maxMs;
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
