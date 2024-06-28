
#ifndef SENTINEL_TIMER_H
#define SENTINEL_TIMER_H

#include <chrono>

class Timer {
public:
    Timer(int maxDurationMs): startTime(std::chrono::high_resolution_clock::now()), _maxDurationMs(maxDurationMs) {};
    Timer() : startTime(std::chrono::high_resolution_clock::now()) {};
    bool isTimeout() const;
    int getMs()const;
private:
    std::chrono::high_resolution_clock::time_point startTime;
    int _maxDurationMs;
};


#endif //SENTINEL_TIMER_H
