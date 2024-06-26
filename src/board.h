
#ifndef SENTINEL_BOARD_H
#define SENTINEL_BOARD_H

#include <map>
#include <string>
#include <vector>
#include <bit_ops.h>
#include <move.h>
#include <state.h>
#include <unordered_map>
#include <pst.h>

class Board {
    static inline std::map<char, int> pieceIndexMap = {{'p',0}, {'n', 1}, {'b', 2}, {'r',3}, {'q',4}, {'k', 5}};
    static inline std::map<int, char> reversedPieceIndexMap = {{0, 'p'}, {1, 'n'}, {2, 'b'}, {3, 'r'}, {4, 'q'}, {5, 'k'}};
    // lazy mapping.
    static inline std::map<char, int> files = {{'a', 0}, {'b', 1}, {'c', 2}, {'d',3}, {'e', 4}, {'f', 5}, {'g', 6}, {'h',7}};
    static inline std::map<char, int> ranks = {{'1',7}, {'2',6},{'3',5},{'4',4}, {'5',3}, {'6',2}, {'7',1}, {'8',0}};

public:

    enum pieceType{
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };

    enum pieceColor{
        WHITE,
        BLACK
    };

    // Bitboards
    uint64_t whitePieces[6]; // make it easy
    uint64_t blackPieces[6];

    // enPassantSquare is set to -1 if there is no enpassant
    int halfMove, fullMove, enPassantSquare;
    // true -> white, false -> black
    bool whoPlay;

    // WHITE, BLACK {queen, king}
    static inline constexpr int K_CASTLE = 1;
    static inline constexpr int Q_CASTLE = 0;
    std::array<std::array<bool, 2>,2> castling;

    // static array for a performance
    // Simple hack for multithreading <-> make it 2D :ez:
    static constexpr int MAX_DEPTH = 1000; // Fuck it, we ball
    static inline State STACK[MAX_DEPTH + 1];

    // i love draws by repetitions.
    static inline uint64_t threeFoldRepetition[MAX_DEPTH];
    static inline int repetitionIndex = 0;

    static inline std::array<int, 2> fiftyMoveRule = {0,0};

    uint64_t zobristKey;
    int piecesTotal;
    static constexpr int END_GAME_PIECE_MAX = 15;

    /***
     * Loads a fen to a _board.
     * @param FEN
     */
    void loadFEN(const std::string FEN);

    /***
     * Initializes all bitboards to an empty state.
     */
    Board();

    /***
     * @param type
     * @param color
     * @return bitboard for a request.
     */
    const uint64_t& getPieceBitboard(pieceType type, pieceColor color) const;

    /***
     * Makes a move on a _board.
     * Validates a move - from a pseudolegal movegen.
     * Non const -> we will change move::capturePiece.
     * @param move
     * @param depth Depth of a current state in search/movegen <-> static size stack (array)
     * @return if move was actually played - because of pseudolegal movegen
     */
    bool makeMove(const Move& move);

    /***
     * Undo a move.
     * @param move
     * @param depth Depth of a current state in search/movegen <-> static size stack (array)
     */
    void undoMove(const Move& move);

    /***
     * Simple _board print of a current state.
     */
    void printBoard() const;

    /***
     * Evaluation function of a current position.
     * @note whoplay = 1 (white), whoplay = -1 (black)
     * @return
     */
    int eval();


    /***
     * Checks, if there is a draw on the _board.
     * @return
     */
    bool isDraw();

    inline std::pair<Board::pieceType, bool> getPieceTypeFromSQ(int square, const uint64_t* bbs) const{
        for(int j = 0; j < 6; j++){
            if(bit_ops::getNthBit(bbs[j], square)) return {(Board::pieceType)j, true};
        }
        return {(Board::pieceType)0, false};
    }


    inline Board::pieceType getPieceType(int square){
        for(int j = 0; j < 6; j++){
            if(bit_ops::getNthBit(whitePieces[j], square) || bit_ops::getNthBit(blackPieces[j], square)) return (Board::pieceType)j;
        }
        assert(false);
    }

private:

    void initPieces(uint64_t* pieces);
    /***
     * Side evaluation
     * -> PST (custom+inspired) - DONE
     * -> Mobility - TODO
     * -> maybe king safety/pawn structure + passed pawns. - TODO
     * @param bbs
     * @return eval.
     */
    int evalSide(uint64_t* bbs, bool white) const;
    bool isInsufficientMaterial(uint64_t* bbs) const;
    bool isSquareAttacked(int square, bool isWhiteEnemy);

    void push(bool setEnPassant, State &currentState);

    bool isThreeFoldRepetition() const;
};


#endif //SENTINEL_BOARD_H
