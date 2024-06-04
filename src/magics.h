#ifndef SENTINEL_MAGICS_H
#define SENTINEL_MAGICS_H


#include <cstdint>
#include <vector>
#include <unordered_map>
#include <bitboard.h>

/* Example, Rook on e4:
 *
 *     Precalculated       Input blockerBitboard
 *    The blocker mask        A blocker board         The move board
 *    0 0 0 0 0 0 0 0         0 0 0 0 0 0 0 0         0 0 0 0 0 0 0 0
 *    0 0 0 0 1 0 0 0         0 0 0 0 1 0 0 0         0 0 0 0 0 0 0 0
 *    0 0 0 0 1 0 0 0         0 0 0 0 0 0 0 0         0 0 0 0 0 0 0 0
 *    0 0 0 0 1 0 0 0         0 0 0 0 1 0 0 0         0 0 0 0 1 0 0 0
 *    0 1 1 1 0 1 1 0         0 1 1 0 0 0 0 0         0 0 1 1 0 1 1 1
 *    0 0 0 0 1 0 0 0         0 0 0 0 0 0 0 0         0 0 0 0 1 0 0 0
 *    0 0 0 0 1 0 0 0         0 0 0 0 1 0 0 0         0 0 0 0 1 0 0 0
 *    0 0 0 0 0 0 0 0         0 0 0 0 0 0 0 0         0 0 0 0 0 0 0 0
 */
class magics {
    // Blocked masks for each square.
    static inline uint64_t ROOK_BLOCKERS[64];
    static inline uint64_t BISHOP_BLOCKERS[64];

    // for each square map<{key, moves}>
    static inline std::unordered_map<uint64_t, uint64_t> ROOK_TABLE[64];
    static inline std::unordered_map<uint64_t, uint64_t> BISHOP_TABLE[64];

    // magic values for each square with shift
    static inline uint64_t ROOK_MAGICS[64];
    static inline uint64_t ROOK_MAGICS_SHIFT[64];

    // magic values for each square with shift
    static inline uint64_t BISHOP_MAGICS[64];
    static inline uint64_t BISHOP_MAGICS_SHIFT[64];
public:
    static void init(){
        generateBishopBlockers();
        generateRookBlockers();
    }


private:
    /***
     * Simple blocker generation using 2D array converted into bitboard -> it will be called only once for a program run.
     */
    static void generateRookBlockers(){
        std::vector<std::pair<int, int>> rookDirections = {{0,1},{1,0},{-1,0},{0,-1}};

        Bitboard tmp;
        for(int rank = 0; rank < 8; rank++ ){
            for(int file = 0; file < 8; file++){
                int square = rank * 8 + file;
                tmp.value = 0ULL;
                auto moves = generateMovesForDirections(rookDirections, rank, file);
                for(auto move: moves) tmp.setNthBit(move.first*8 + move.second);
                ROOK_BLOCKERS[square] = tmp.value;
            }
        }
    }

    /***
     * Simple blocker generation using 2D array converted into bitboard -> it will be called only once for a program run.
     */
    static void generateBishopBlockers(){
        std::vector<std::pair<int, int>> bishopDirections = {{1,1}, {-1,-1}, {1,-1}, {-1,1}};
        Bitboard tmp;
        for(int rank = 0; rank < 8; rank++ ){
            for(int file = 0; file < 8; file++){
                tmp.value = 0ULL;
                int square = rank * 8 + file;
                auto moves = generateMovesForDirections(bishopDirections, rank, file);
                for(auto move: moves) tmp.setNthBit(move.first*8 + move.second);
                tmp.printBoard();
                BISHOP_BLOCKERS[square] = tmp.value;
            }
        }
    }

    static std::vector<std::pair<int, int>> generateMovesForDirections(const std::vector<std::pair<int,int>>& directions, int rank, int file){
        std::vector<std::pair<int, int>> result;
        for(const auto& direction : directions){
            int rankTmp = rank + direction.second;
            int fileTmp = file + direction.first;
            bool added = false;
            while(rankTmp >= 0 && rankTmp <= 7 && fileTmp >= 0 && fileTmp <= 7){
                result.push_back({rankTmp, fileTmp});
                rankTmp += direction.second;
                fileTmp += direction.first;
                added = true;
            }
            // last move was the end of a board, remove it -> pointless.
            if(added) result.pop_back();
        }
        return result;
    }


    static void bruteforceMagics(){
        // TODO.
    }
};

#endif //SENTINEL_MAGICS_H
