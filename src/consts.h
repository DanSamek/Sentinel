#ifndef SENTINEL_CONSTS_H
#define SENTINEL_CONSTS_H

namespace Sentinel{
    static constexpr int CHECKMATE = 1000000;
    static constexpr int CHECKMATE_LOWER_BOUND = 1000000 - 1000;
    static inline constexpr int MAX_DEPTH = 128;
    static constexpr int WIN_BOUND = 1000000 - 128;
    static inline Move NO_MOVE = Move();
}
#endif //SENTINEL_CONSTS_H
