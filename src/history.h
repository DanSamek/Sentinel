#ifndef SENTINEL_MOVEHISTORY_H
#define SENTINEL_MOVEHISTORY_H

#include "move.h"
#include "consts.h"

namespace Sentinel{

    struct History {

        // Killer moves, that did beta cutoffs, use them in move order.
        Move killerMoves[MAX_DEPTH][2];
        int history[64][64];

        // Some moves can have natural response.
        Move counterMoves[64][64];

        static inline constexpr int MAX_HISTORY = 65536;

        inline void storeKillerMove(int ply, const Move& move){
            killerMoves[ply][1] = killerMoves[ply][0];
            killerMoves[ply][0] = move;
        }

        inline void updateHistory(const Move& move, int bonus){
            // History gravity.
            auto clampedBonus = std::clamp(bonus, -MAX_HISTORY, MAX_HISTORY);
            history[move.fromSq][move.toSq] += clampedBonus - history[move.fromSq][move.toSq] * abs(clampedBonus) / MAX_HISTORY;
        }

        inline void storeCounterMove(const Move& prevMove, const Move& move){
            counterMoves[prevMove.fromSq][prevMove.toSq] = move;
        }

        inline void init(){
            for (auto& killers : killerMoves) {
                std::fill(std::begin(killers), std::end(killers), Move());
            }

            for (auto& cm : counterMoves) {
                std::fill(std::begin(cm), std::end(cm), Move());
            }

            for (auto& row : history) {
                std::fill(std::begin(row), std::end(row), 0);
            }
        }

    };
}

#endif //SENTINEL_MOVEHISTORY_H
