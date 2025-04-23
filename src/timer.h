#include <chrono>

#ifndef SENTINEL_TIMER_H
#define SENTINEL_TIMER_H

namespace Sentinel{

    class Timer {
    public:
        Timer(double maxDurationMs, bool infinity) :
            startTime(std::chrono::high_resolution_clock::now()),
            _maxDurationMs(maxDurationMs),
            _infinity(infinity) {};

        Timer() : startTime(std::chrono::high_resolution_clock::now()) {};
        bool isTimeout() const;
        int getMs()const;
    private:
        std::chrono::high_resolution_clock::time_point startTime;
        double _maxDurationMs;
        bool _infinity;
    };

}
#endif //SENTINEL_TIMER_H
