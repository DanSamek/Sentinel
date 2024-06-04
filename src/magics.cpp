#include "magics.h"
#include "set"

void magics::init() {
    generateBishopBlockers();
    generateRookBlockers();
    initMagics();
}

uint64_t magics::getSlidingMoves(uint64_t blockers, int square, bool rook){
    uint64_t hashBlockers = blockers & (rook ? ROOK_BLOCKERS[square] : BISHOP_BLOCKERS[square]);
    uint64_t magics = rook ? ROOK_MAGICS[square] : BISHOP_MAGICS[square];
    uint64_t hash;
    __builtin_umull_overflow(hashBlockers, magics, &hash);
    uint64_t index = (hash >> (uint64_t)(64ULL - (rook ? ROOK_MAGICS_SHIFT[square] : BISHOP_MAGICS_SHIFT[square])));
    return ROOK_TABLE[square][index];
}

std::vector<uint64_t> magics::getMagics(int file, int rank, uint64_t* sliderBlockers, bool rook, uint64_t magic){
    int square = rank * 8 + file;
    auto allBlockers = generateAllBlockerCombinations(sliderBlockers[square]);
    auto table = tryBuildTable(sliderBlockers[square], file, rank, rook, magic, allBlockers);
    return table;
}

uint64_t magics::magic_index(uint64_t currentBlockers, uint64_t tableBlocker, int indexBits, uint64_t magics){
    uint64_t blockers = currentBlockers & tableBlocker;
    uint64_t hash;
    __builtin_umull_overflow(blockers, magics, &hash);
    uint64_t index = (hash >> (uint64_t)(64 - indexBits));
    return index;
}


void magics::initMagics() {
    for(int rank = 0; rank < 8; rank++ ) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            ROOK_TABLE[square] = getMagics(file, rank, ROOK_BLOCKERS, true, ROOK_MAGICS[square]);
            assert(ROOK_TABLE[square].size() != 0);
        }
    }

    for(int rank = 0; rank < 8; rank++ ) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            BISHOP_TABLE[square] = getMagics(file, rank, BISHOP_BLOCKERS, false, BISHOP_MAGICS[square]);
            assert(BISHOP_TABLE[square].size() != 0);
        }
    }
}

void magics::generateRookBlockers(){
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


void magics::generateBishopBlockers(){
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

std::vector<std::pair<int, int>> magics::generateMovesForDirections(const std::vector<std::pair<int,int>>& directions, int rank, int file){
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

std::vector<uint64_t> magics::generateAllBlockerCombinations(uint64_t bitboard){
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
    assert(result.size() == (size_t(1 << bits.size())));

    return result;
}

uint64_t magics::generateSliderMoves(int file, int rank, uint64_t bitboard, const std::vector<std::pair<int, int>> movement){
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


std::pair<std::vector<uint64_t>, uint64_t> magics::findMagics(int file, int rank, uint64_t* sliderBlockers, bool rook){
    int square = rank * 8 + file;
    auto allBlockers = generateAllBlockerCombinations(sliderBlockers[square]);
    while(true){
        auto magic = randUInt64() & randUInt64() & randUInt64();
        auto table = tryBuildTable(sliderBlockers[square], file, rank, rook, magic, allBlockers);
        if(table.size() != 0){
            return {table, magic};
        }
    }
}

std::vector<uint64_t> magics::tryBuildTable(uint64_t blockerBitBoard, int file, int rank, bool rook,
                                           uint64_t magic, const std::vector<uint64_t>& allBlockers){
    int indexBits = rook ?  ROOK_MAGICS_SHIFT[rank * 8 + file] : BISHOP_MAGICS_SHIFT[rank * 8 + file];
    std::vector<uint64_t> table(1 << indexBits, 0);
    for(auto& blocker: allBlockers){
        auto moves = generateSliderMoves(file, rank, blocker, rook ? rookDirections : bishopDirections);
        auto index = magic_index(blocker, blockerBitBoard, indexBits, magic);
        auto item = table[index];

        if(item == 0ULL) table[index] = moves;
        else if(moves != item) return {};
    }
    return table;
}


uint64_t magics::randUInt64(){
    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_int_distribution<uint64_t> distribution(0, std::numeric_limits<uint64_t>::max());
    return distribution(generator);
}

void magics::generateMagics(){
    std::vector<uint64_t> RESULT(64,0);
    for(int rank = 0; rank < 8; rank++ ){
        for(int file = 0; file < 8; file++) {
            auto result = findMagics(file, rank, ROOK_BLOCKERS, true);
            RESULT[rank * 8 + file] = result.second;
        }
    }
    for(auto item: RESULT){
        std::cout << std::hex << "0x" << item << ",";
    }
    RESULT.clear();

    RESULT = std::vector<uint64_t>(64,0);
    std::cout << std::endl;

    for(int rank = 0; rank < 8; rank++ ){
        for(int file = 0; file < 8; file++) {
            auto result = findMagics(file, rank, BISHOP_BLOCKERS, false);
            RESULT[rank * 8 + file] = result.second;
        }
    }
    for(auto item: RESULT){
        std::cout << std::hex << "0x" << item << ",";
    }
    std::cout << std::endl;
}