
#ifndef SENTINEL_BOARD_H
#define SENTINEL_BOARD_H

#include <map>
#include <string>
#include <vector>
#include <bit_ops.h>
#include <move.h>
#include <state.h>
#include <unordered_map>

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

    // simple piece eval.
    // Todo maybe add endgame/middle game tables?
    static constexpr int PIECE_EVAL_EARLY[6] = {100, 285, 310, 500, 930, 10000};

    // Bitboards
    uint64_t whitePieces[6]; // make it easy
    uint64_t blackPieces[6];

    // enPassantSquare is set to -1 if there is no enpassant
    int halfMove, fullMove, enPassantSquare;
    // true -> white, false -> black
    bool whoPlay;

    // WHITE, BLACK {queen, king}
    static inline int K_CASTLE = 1;
    static inline int Q_CASTLE = 0;
    std::array<std::array<bool, 2>,2> castling;

    // static array for a performance
    // Simple hack for multithreading <-> make it 2D :ez:
    static constexpr int MAX_DEPTH = 1000; // Fuck it, we ball
    static inline State STACK[MAX_DEPTH + 1];

    // i love draws by repetitions.
    static inline std::unordered_map<uint64_t, int> threeFoldRepetition;

    static inline std::array<int, 2> fiftyMoveRule = {0,0};

    uint64_t zobristKey;

    /***
     * Loads a fen to a board.
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
     * Makes a move on a board.
     * Non const -> we will change move::capturePiece.
     * @param move
     * @param depth Depth of a current state in search/movegen <-> static size stack (array)
     */
    void makeMove(const Move& move, bool updateHash = true);

    /***
     * Undo a move.
     * @param move
     * @param depth Depth of a current state in search/movegen <-> static size stack (array)
     */
    void undoMove(const Move& move, bool updateHash = true);

    /***
     * Simple board print of a current state.
     */
    void printBoard();


    /***
     * Evaluation function of a current position.
     * @note whoplay = 1 (white), whoplay = -1 (black)
     * @return
     */
    int eval();


    /***
     * Checks, if there is a draw on the board.
     * @return
     */
    bool isDraw();

    inline std::pair<Board::pieceType, bool> getPieceTypeFromSQ(int square, const uint64_t* bbs) const{
        for(int j = 0; j < 6; j++){
            if(bit_ops::getNthBit(bbs[j], square)) return {(Board::pieceType)j, true};
        }
        return {(Board::pieceType)0, false};
    }

private:

    void initPieces(uint64_t* pieces);
    /***
     * Side evaluation
     * -> Piece count
     * TODO
     * -> PESTO's
     * -> Mobility
     * -> maybe king safety/pawn structure + passed pawns.
     * @param bbs
     * @return eval.
     */
    int evalSide(uint64_t* bbs);

    bool isInsufficientMaterial(uint64_t* bbs);
};


#endif //SENTINEL_BOARD_H
