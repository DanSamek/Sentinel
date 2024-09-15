#ifndef SENTINEL_DATAGEN_H
#define SENTINEL_DATAGEN_H

#include <atomic>

class Datagen {
public:
    Datagen(int threadCount, int searchDepth = 9) : _threadCnt(threadCount), _searchDepth(searchDepth) {};
    void run();
private:
    void runWorker(int maxDepth, int threadId);
    int _threadCnt = 0;
    int _searchDepth;
    std::atomic<bool> _stopSignal;
    std::mutex _coutMutex;
};

#endif //SENTINEL_DATAGEN_H
