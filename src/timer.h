
#ifndef SENTINEL_TIMER_H
#define SENTINEL_TIMER_H

#include <chrono>

class Timer {
public:
    Timer(int maxDurationMs): startTime(std::chrono::high_resolution_clock::now()), _maxDurationMs(maxDurationMs) {};
    Timer() = default;
    bool isTimeout() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
        return elapsed.count() > _maxDurationMs;
    }

private:
    std::chrono::high_resolution_clock::time_point startTime;
    int _maxDurationMs;
};


#endif //SENTINEL_TIMER_H
