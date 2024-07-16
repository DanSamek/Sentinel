#include "timer.h"

bool Timer::isTimeout() const {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = now - startTime;
    return (elapsed.count() * 1000 ) >= _maxDurationMs;
}

int Timer::getMs() const{
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
    return elapsed.count();
}
