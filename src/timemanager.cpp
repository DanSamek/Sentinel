#include <algorithm>
#include "timemanager.h"

int Timemanager::getSearchTime(int timeRemaining, int increment, bool exact) {
    const auto minMs = 5;

    auto msCanBeUsed = exact ? timeRemaining : timeRemaining / 20;
    // increment
    msCanBeUsed += increment / 2 + increment / 4;

    // if we are out of time, clamp it.
    if(msCanBeUsed >= timeRemaining && !exact){
        msCanBeUsed = std::clamp(msCanBeUsed, minMs, timeRemaining / 20);
    }
    return msCanBeUsed;
}