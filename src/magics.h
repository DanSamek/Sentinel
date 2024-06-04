#ifndef SENTINEL_MAGICS_H
#define SENTINEL_MAGICS_H


#include <cstdint>
#include <vector>
#include <unordered_map>
#include <bitboard.h>
#include <cassert>
#include <algorithm>
#include <valarray>
#include <random>

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
public:
    /***
     * Initialization of all magics
     */
    static void init();

    /***
     * Gets valid moves for current piece.
     * @param blockers current blockers
     * @param square square of piece
     * @param rook if its rook
     * @return
     */
    static uint64_t getSlidingMoves(uint64_t blockers, int square, bool rook);
private:

    // Blocked masks for each square.
    static inline uint64_t ROOK_BLOCKERS[64];
    static inline uint64_t BISHOP_BLOCKERS[64];

    static inline std::vector<uint64_t> ROOK_TABLE[64];
    static inline std::vector<uint64_t> BISHOP_TABLE[64];

    // magic values for each square with shift
    static inline uint64_t ROOK_MAGICS[64] = {
            0x1080054000217080,0x800040043080,0x1000808000400026,0x200400080022080,0x420400088800020,0x80042001444008,0x9600800045006f00,0x120908009002041,
            0xc0001004402002,0x200c400020100040,0x810040006100,0x110004440006000,0x2004102002280,0xc440d00120024000,0x1a0014010012040,0x89001020c202,
            0x700084411002001,0x802000809000,0x2310008020008030,0x2028200204210,0x4042018a001020,0x40442001010016,0x14406200801200,0xc002002140500882,
            0x80048800801000,0x8800800100082,0xa0012002041,0x1000a202000810c0,0x820801002800800,0x300019010020,0x40100102b0082100,0xa00482100b0001d,
            0x180040002080080,0x4006001004200a00,0x6110025000800,0xa002000a00201004,0x1000451000800,0x502002051460008,0xa8a06200e000a00,0x2001000410060801,
            0x1100080400010002,0x90800200800400,0x2401010002840008,0xe20240801102040,0x604008010410,0x8044010080120,0x1000020004008080,0x8a1000208440001,
            0x22000800a1120004,0xa081001100020004,0x10800400018a3008,0x4a00020400108801,0x2042090c001008,0x1000200110014,0x5000084210810400,0x105010804900204,
            0x100022482410002,0x1230010c0820500,0x1000820001204984,0x22060502000a8044,0x34240042000191,0xa1104082000c,0x1200006401018200,0x4001864024010082,
    };

    static inline uint64_t ROOK_MAGICS_SHIFT[64] = {  12, 11, 11, 11, 11, 11, 11, 12,
                                                      11, 10, 10, 10, 10, 10, 10, 11,
                                                      11, 10, 10, 10, 10, 10, 10, 11,
                                                      11, 10, 10, 10, 10, 10, 10, 11,
                                                      11, 10, 10, 10, 10, 10, 10, 11,
                                                      11, 10, 10, 10, 10, 10, 10, 11,
                                                      11, 10, 10, 10, 10, 10, 10, 11,
                                                      12, 11, 11, 11, 11, 11, 11, 12};

    // magic values for each square with shift
    static inline uint64_t BISHOP_MAGICS[64] = {
            0x4010890208020820,0x8094200810413052,0x1028402006100200,0x41000c3100910,0x2021305010482005,0x1002060e20004100,0x20610828400040,0x4640118180208,
            0x50011802908520,0x13841488024501,0x8204202080200,0x1008400208011a,0x210801108014440a,0x5010b10802000,0x2a020201044440,0x230000808488200a,
            0x42440042800402,0x1048090204110050,0x8001021c14108,0xa10480094080010,0xa2088082c0100400,0x906884410010604, 0xc0100c40c241000,0x50000000240a2800
            ,0x1960a0a00000011,0x4050a001820,0x28822050c030000,0x11040800c0e02040, 0x4200800210050,0x1000006018082d00,0x1031002020981008,0x292004000841100,
            0x2c042000080042,0x28020a0210000080, 0x20801402a00014,0x801001011004000,0x10c040400009010,0xb24080100c14c02,0x80a88090202a0100,0x10504040882200,
            0x8e104208c00c010,0x1620815040100,0x400808021000,0x442a060112480201,0x502009a00090050,0x80d3011000821101, 0x10208401020600,0x2000408101104,
            0xa010150100d0801,0xc4080ac104030,0x2014008216190420,0x4c0124000300c800, 0x8008188100042113,0x48a0081080812100,0x1041480504028200,0xc00881010020050,
            0x420950021030,0x8010204201942000, 0x44408a00520824,0x21080434405a0,0x2c18208820010100,0x3408080098600a80,0x84100400448000,0x8010300128008012,
    };
    static inline uint64_t BISHOP_MAGICS_SHIFT[64] = {
            6, 5, 5, 5, 5, 5, 5, 6,
            5, 5, 5, 5, 5, 5, 5, 5,
            5, 5, 7, 7, 7, 7, 5, 5,
            5, 5, 7, 9, 9, 7, 5, 5,
            5, 5, 7, 9, 9, 7, 5, 5,
            5, 5, 7, 7, 7, 7, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 5,
            6, 5, 5, 5, 5, 5, 5, 6
    };


    static std::vector<uint64_t> getMagics(int file, int rank, uint64_t* sliderBlockers, bool rook, uint64_t magic);
    static uint64_t magic_index(uint64_t currentBlockers, uint64_t tableBlocker, int indexBits, uint64_t magics);
    /***
    * Initialization of movetables for rooks, bishops.
    */
    static void initMagics();

    static inline std::vector<std::pair<int, int>> rookDirections = {{0,1},{1,0},{-1,0},{0,-1}};
    /***
     * Simple blocker generation using 2D array converted into bitboard -> it will be called only once for a program run.
     */
    static void generateRookBlockers();

    static inline std::vector<std::pair<int, int>> bishopDirections = {{1,1}, {-1,-1}, {1,-1}, {-1,1}};
    /***
     * Simple blocker generation using 2D array converted into bitboard -> it will be called only once for a program run.
     */
    static void generateBishopBlockers();

    /***
     * For rook and bishop move generation.
     * @return moves.
     */
    static std::vector<std::pair<int, int>> generateMovesForDirections(const std::vector<std::pair<int,int>>& directions, int rank, int file);

    /***
     * Generate all subsets of *_BLOCKERS (possible combinations of blocks).
     * Than for this generate all possible moves bitboard.
    */
    static std::vector<uint64_t> generateAllBlockerCombinations(uint64_t bitboard);

    /***
     * Generates all possible moves for current board from file, rank.
     */
    static uint64_t generateSliderMoves(int file, int rank, uint64_t bitboard, const std::vector<std::pair<int, int>> movement);

    /***
     * Bruteforce search for magic numbers.
     * @return moves, magicNumber
     */
    static std::pair<std::vector<uint64_t>, uint64_t> findMagics(int file, int rank, uint64_t* sliderBlockers, bool rook);

    /***
     * Part of bruteforcing part for magic numbers
     */
    static std::vector<uint64_t> tryBuildTable(uint64_t blockerBitBoard, int file, int rank, bool rook, uint64_t magic, const std::vector<uint64_t>& allBlockers);

    /***
     * Random uint64t number for magics.
     */
    static uint64_t randUInt64();

    /***
     * For each square, for each blocker combination -> move board.
     * find magic number and shifts for each square, minimum maximal key for squares.
     */
    static void generateMagics();
};

#endif //SENTINEL_MAGICS_H
