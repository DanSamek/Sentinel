
#include <board.h>
#include <bitboard.h>
#include <vector>
#include <cassert>

struct boardTests {

    static void setMultipleBits(std::vector<int> bits, Bitboard& b){
        b.value = 0ULL;
        for(auto bit: bits) b.setNthBit(bit);
    }

    static bool sameCastling(bool tmp1[2][2], bool tmp2[2][2]){
        for(int j = 0; j < 2; j++) for(int i = 0; i < 2; i++) if(tmp1[i][j] != tmp2[i][j]) return false;
        return true;
    }

    // Loading fen test and checking all bitboards, etc.
    static void testBoard(){
        Board board;
        board.loadFEN("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");

        bool tmp[2][2] = {{true, true}, {true, true}};
        assert(sameCastling(board.castling ,tmp));
        assert(board.enPassantSquare == 44);
        assert(board.halfMove == 0);
        assert(board.fullMove == 1);
        // board validation with bitboards.

        Bitboard tmpBit;
        setMultipleBits({8,9,10,11,12,13,14,15}, tmpBit);
        assert(board.getPieceBitboard(Bitboard::PAWN, Bitboard::BLACK).value == tmpBit.value);

        setMultipleBits({36,48,49,50,51,53,54,55}, tmpBit);
        assert(board.getPieceBitboard(Bitboard::PAWN, Bitboard::WHITE).value == tmpBit.value);

        setMultipleBits({60}, tmpBit);
        assert(board.getPieceBitboard(Bitboard::KING, Bitboard::WHITE).value == tmpBit.value);

        setMultipleBits({0,7}, tmpBit);
        assert(board.getPieceBitboard(Bitboard::ROOK, Bitboard::BLACK).value == tmpBit.value);

        board.loadFEN("8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - - 99 50");
        bool tmp2[2][2] = {{false, false}, {false, false}};
        assert(board.enPassantSquare  == -1);
        assert(board.halfMove == 99);
        assert(board.fullMove == 50);
        assert(sameCastling(board.castling ,tmp2));

        setMultipleBits({47}, tmpBit);
        assert(board.getPieceBitboard(Bitboard::KING, Bitboard::WHITE).value == tmpBit.value);

        setMultipleBits({13}, tmpBit);
        assert(board.getPieceBitboard(Bitboard::KING, Bitboard::BLACK).value == tmpBit.value);

        setMultipleBits({19,25,28,31,32,37}, tmpBit);
        assert(board.getPieceBitboard(Bitboard::PAWN, Bitboard::BLACK).value == tmpBit.value);

        setMultipleBits({27,33,36,39,40,45}, tmpBit);
        assert(board.getPieceBitboard(Bitboard::PAWN, Bitboard::WHITE).value == tmpBit.value);


        // WHITE, BLACK {queen, king}
        board.loadFEN("4k2r/6r1/8/8/8/8/3R4/R3K3 w Qk - 0 1");
        bool tmp3[2][2] = {{true, false}, {false, true}};
        assert(board.enPassantSquare  == -1);
        assert(board.halfMove == 0);
        assert(board.fullMove == 1);
        assert(sameCastling(board.castling ,tmp3));


        setMultipleBits({60}, tmpBit);
        assert(board.getPieceBitboard(Bitboard::KING, Bitboard::WHITE).value == tmpBit.value);

        setMultipleBits({4}, tmpBit);
        assert(board.getPieceBitboard(Bitboard::KING, Bitboard::BLACK).value == tmpBit.value);

        setMultipleBits({7,14}, tmpBit);
        assert(board.getPieceBitboard(Bitboard::ROOK, Bitboard::BLACK).value == tmpBit.value);

        setMultipleBits({56,51}, tmpBit);
        assert(board.getPieceBitboard(Bitboard::ROOK, Bitboard::WHITE).value == tmpBit.value);


        assert(board.getPieceBitboard(Bitboard::QUEEN, Bitboard::WHITE).value == 0ull);
        assert(board.getPieceBitboard(Bitboard::KNIGHT, Bitboard::WHITE).value == 0ull);
        assert(board.getPieceBitboard(Bitboard::BISHOP, Bitboard::WHITE).value == 0ull);
        assert(board.getPieceBitboard(Bitboard::PAWN, Bitboard::WHITE).value == 0ull);

        assert(board.getPieceBitboard(Bitboard::QUEEN, Bitboard::BLACK).value == 0ull);
        assert(board.getPieceBitboard(Bitboard::KNIGHT, Bitboard::BLACK).value == 0ull);
        assert(board.getPieceBitboard(Bitboard::BISHOP, Bitboard::BLACK).value == 0ull);
        assert(board.getPieceBitboard(Bitboard::PAWN, Bitboard::BLACK).value == 0ull);

        std::cout << "board tests: DONE" << std::endl;
    }
};