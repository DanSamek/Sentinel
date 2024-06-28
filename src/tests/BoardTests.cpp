
#include <board.h>
#include <vector>
#include <cassert>
#include <bit_ops.h>

struct BoardTests {

    static void setMultipleBits(std::vector<int> bits, uint64_t & b){
        b= 0ULL;
        for(auto bit: bits) bit_ops::setNthBit(b, bit);
    }

    static bool sameCastling(std::array<std::array<bool,2>,2> tmp1, std::array<std::array<bool,2>,2> tmp2){
        for(int j = 0; j < 2; j++) for(int i = 0; i < 2; i++) if(tmp1[i][j] != tmp2[i][j]) return false;
        return true;
    }

    // Loading fen test and checking all bitboards, etc.
    static void testBoard(){
        Board board;
        board.loadFEN("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");

        std::array<std::array<bool, 2>, 2> tmp;
        tmp[0] = {true, true};
        tmp[1]= {true, true};
        assert(sameCastling(board.castling ,tmp));
        assert(board.enPassantSquare == 44);
        assert(board.halfMove == 0);
        assert(board.fullMove == 1);
        // _board validation with bitboards.

        uint64_t tmpBit;
        setMultipleBits({8,9,10,11,12,13,14,15}, tmpBit);
        assert(board.getPieceBitboard(Board::PAWN, Board::BLACK) == tmpBit );

        setMultipleBits({36,48,49,50,51,53,54,55}, tmpBit);
        assert(board.getPieceBitboard(Board::PAWN, Board::WHITE)  == tmpBit );

        setMultipleBits({60}, tmpBit);
        assert(board.getPieceBitboard(Board::KING, Board::WHITE)  == tmpBit );

        setMultipleBits({0,7}, tmpBit);
        assert(board.getPieceBitboard(Board::ROOK, Board::BLACK)  == tmpBit );

        board.loadFEN("8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - - 99 50");
        std::array<std::array<bool,2>,2> tmp2;
        tmp2[0]= {false, false};
        tmp2[1]= {false, false};
        assert(board.enPassantSquare  == -1);
        assert(board.halfMove == 99);
        assert(board.fullMove == 50);
        assert(sameCastling(board.castling ,tmp2));

        setMultipleBits({47}, tmpBit);
        assert(board.getPieceBitboard(Board::KING, Board::WHITE)  == tmpBit );

        setMultipleBits({13}, tmpBit);
        assert(board.getPieceBitboard(Board::KING, Board::BLACK)  == tmpBit );

        setMultipleBits({19,25,28,31,32,37}, tmpBit);
        assert(board.getPieceBitboard(Board::PAWN, Board::BLACK)  == tmpBit );

        setMultipleBits({27,33,36,39,40,45}, tmpBit);
        assert(board.getPieceBitboard(Board::PAWN, Board::WHITE)  == tmpBit );


        // WHITE, BLACK {queen, king}
        board.loadFEN("4k2r/6r1/8/8/8/8/3R4/R3K3 w Qk - 0 1");
        std::array<std::array<bool,2>,2> tmp3;
        tmp3[0] = {true, false};
        tmp3[1] = {false, true};
        assert(board.enPassantSquare  == -1);
        assert(board.halfMove == 0);
        assert(board.fullMove == 1);
        assert(sameCastling(board.castling ,tmp3));


        setMultipleBits({60}, tmpBit);
        assert(board.getPieceBitboard(Board::KING, Board::WHITE)  == tmpBit );

        setMultipleBits({4}, tmpBit);
        assert(board.getPieceBitboard(Board::KING, Board::BLACK)  == tmpBit );

        setMultipleBits({7,14}, tmpBit);
        assert(board.getPieceBitboard(Board::ROOK, Board::BLACK)  == tmpBit );

        setMultipleBits({56,51}, tmpBit);
        assert(board.getPieceBitboard(Board::ROOK, Board::WHITE)  == tmpBit );


        assert(board.getPieceBitboard(Board::QUEEN, Board::WHITE)  == 0ull);
        assert(board.getPieceBitboard(Board::KNIGHT, Board::WHITE)  == 0ull);
        assert(board.getPieceBitboard(Board::BISHOP, Board::WHITE)  == 0ull);
        assert(board.getPieceBitboard(Board::PAWN, Board::WHITE)  == 0ull);

        assert(board.getPieceBitboard(Board::QUEEN, Board::BLACK)  == 0ull);
        assert(board.getPieceBitboard(Board::KNIGHT, Board::BLACK)  == 0ull);
        assert(board.getPieceBitboard(Board::BISHOP, Board::BLACK)  == 0ull);
        assert(board.getPieceBitboard(Board::PAWN, Board::BLACK)  == 0ull);

        std::cout << "_board tests: DONE" << std::endl;
    }
};