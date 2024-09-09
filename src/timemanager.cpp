#include <algorithm>
#include "timemanager.h"

int Timemanager::getSearchTime(int timeRemaining, int increment, bool exact) {
    // minimum search time at least for depth = 1 - for some move.
    const auto minMs = 1;

    auto msCanBeUsed = exact ? timeRemaining : timeRemaining / 18;
    // increment
    msCanBeUsed += increment / 2 + increment / 4;

    // if we are out of time, clamp it.
    if(msCanBeUsed >= timeRemaining && !exact){
        msCanBeUsed = std::clamp(msCanBeUsed, minMs, timeRemaining / 18);
    }
    return msCanBeUsed;
}