#ifndef SENTINEL_MAGICS_H
#define SENTINEL_MAGICS_H


#include <cstdint>
#include <vector>
#include <unordered_map>
#include <bitboard.h>
#include <cassert>
#include <algorithm>
#include <valarray>

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
        bruteforceMagics();
    }


private:
    /***
     * Simple blocker generation using 2D array converted into bitboard -> it will be called only once for a program run.
     */
    static inline std::vector<std::pair<int, int>> rookDirections = {{0,1},{1,0},{-1,0},{0,-1}};
    static void generateRookBlockers(){
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
    static inline std::vector<std::pair<int, int>> bishopDirections = {{1,1}, {-1,-1}, {1,-1}, {-1,1}};
    static void generateBishopBlockers(){
        Bitboard tmp;
        for(int rank = 0; rank < 8; rank++ ){
            for(int file = 0; file < 8; file++){
                tmp.value = 0ULL;
                int square = rank * 8 + file;
                auto moves = generateMovesForDirections(bishopDirections, rank, file);
                for(auto move: moves) tmp.setNthBit(move.first*8 + move.second);
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

    /***
     * Generate all subsets of *_BLOCKERS (possible combinations of blocks).
     * Than for this generate all possible moves bitboard.
    */
    static std::vector<uint64_t> generateAllBlockerCombinations(uint64_t bitboard){
        std::vector<uint64_t> result; // No blockers.
        std::vector<int> bits;

        int move = 0;
        while(move <= 63){
            if((bitboard >> move) & 1) bits.push_back(move);
            move++;
        }

        int n = bits.size();
        int numCombinations = std::pow(2, n);


        for (int i = 0; i < numCombinations; ++i) {
            Bitboard b;
            for (int j = 0; j < n; ++j) {
                if (i & (1 << j)) {
                    b.setNthBit(bits[j]);
                }
            }
            result.push_back(b.value);
        }

        assert(result.size() == ((1 << bits.size())));
        return result;
    }

    static uint64_t generateSliderMoves(int file, int rank, uint64_t bitboard, const std::vector<std::pair<int, int>> movement){
        Bitboard b; b.value = bitboard;

        auto board2d = b.generateBoardFromBitboard();
        std::vector<int> bits;

        for(const auto& direction : movement) {
            int rankTmp = rank + direction.second;
            int fileTmp = file + direction.first;
            while(rankTmp >= 0 && rankTmp <= 7 && fileTmp >= 0 && fileTmp <= 7){
                int square = rankTmp * 8 + fileTmp;
                bits.push_back(square);
                if(board2d[rankTmp][fileTmp]) break;
                rankTmp += direction.second;
                fileTmp += direction.first;
            }
        }

        Bitboard resultB;
        for(auto item: bits) resultB.setNthBit(item);

        return resultB.value;
    }

    /***
     * For each square, for each blocker combination -> move board.
     * find magic number and shifts for each square, minimum maximal key for squares.
     */
    static void bruteforceMagics(){
    }
};

#endif //SENTINEL_MAGICS_H
