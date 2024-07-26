#include <board.h>
#include <magics.h>
#include <movegen.h>


static constexpr int SEE_VALUES[6] = {100, 300, 300, 500, 900, 0};

inline uint64_t MergeBBS(uint64_t* bbs, int cnt){
    uint64_t bb = 0ULL;
    for(int j = 0; j < cnt; j++){
        bb |= bbs[j];
    }
    return bb;
}

inline uint64_t Board::getAttackersForSide(const uint64_t &occupancy, int toSquare, bool color){
    auto bbs = color ? blackPieces : whitePieces;
    uint64_t pawns = bbs[PAWN];
    uint64_t bishops = bbs[BISHOP];
    uint64_t knights = bbs[KNIGHT];
    uint64_t rooks = bbs[ROOK];
    uint64_t queens = bbs[QUEEN];
    uint64_t king = bbs[KING];

    uint64_t attackers = Magics::getRookMoves(occupancy, toSquare) & (rooks | queens);
    attackers |= Magics::getBishopMoves(occupancy, toSquare) & (bishops | queens);
    attackers |= Movegen::KNIGHT_MOVES[toSquare] & knights;
    attackers |= Movegen::PAWN_ATTACK_MOVES[!color][toSquare] & pawns;
    attackers |= Movegen::KING_MOVES[toSquare] & king;
    return attackers;
}

uint64_t Board::getAllAttackers(const uint64_t &occupancy, int toSquare) {
    return getAttackersForSide(occupancy, toSquare, WHITE) | getAttackersForSide(occupancy, toSquare, BLACK);
}

/***
 * Static exchange evaluation (SEE) [Heuristic]
 * "Simulation" of captures on one square.
 * Ordered by least valuable attacker.
 * We dont consider pinned pieces in SEE.
 * @param move
 * @param threshold
 * @return
 */
bool Board::SEE(Move move, int threshold) {
    int from = move.fromSq;
    int to = move.toSq;

    // En-passant handling.
    int target = move.moveType == Move::EN_PASSANT ? PAWN : getPieceType(to);
    int value = -threshold;

    if(target != NO_PIECE) value += SEE_VALUES[target];
    if(value < 0) return false;

    int attacker = getPieceType(from);
    value -= SEE_VALUES[attacker];
    if(value >= 0) return true;

    uint64_t white = MergeBBS(whitePieces, 6);
    uint64_t black = MergeBBS(blackPieces, 6);
    uint64_t all = white | black;

    uint64_t occupied = (all ^ (1ULL << from)) ^ (1ULL << to);
    uint64_t attackers = getAllAttackers(occupied, to);

    uint64_t queens = whitePieces[QUEEN] | blackPieces[QUEEN];
    uint64_t bishops = whitePieces[BISHOP] | blackPieces[BISHOP] | queens;
    uint64_t rooks = whitePieces[ROOK] | blackPieces[ROOK] | queens;

    int colorStart = bit_ops::getNthBit(white, from) ? WHITE : BLACK;
    int colorState = bit_ops::getNthBit(white, from) ? BLACK : WHITE;

    while(true){
        attackers &= occupied;

        uint64_t currentAttackers = (colorState == BLACK ? black : white) & attackers;

        // No more attackers.
        if(currentAttackers == 0ULL) break;

        // Least valuable attacker.
        int pieceType;
        for(pieceType = PAWN; pieceType <= KING; pieceType++){
            if(currentAttackers & (whitePieces[pieceType] | blackPieces[pieceType])) break;
        }

        colorState = !colorState;

        value = -value - 1 - SEE_VALUES[pieceType];
        if(value >= 0){
            if(pieceType == KING && (attackers & (colorState == WHITE ? white : black))) colorState = !colorState;
            break;
        }

        // Make a capture.
        uint64_t tmp = currentAttackers & (whitePieces[pieceType] | blackPieces[pieceType]);
        occupied ^= (1ULL << bit_ops::bitScanForward(tmp));

        // Maybe a capture creates a new attackers on a target square.
        if(pieceType == PAWN || pieceType == QUEEN || pieceType == BISHOP){
            attackers |= Magics::getBishopMoves(occupied, to) & bishops;
        }
        if(pieceType == ROOK || pieceType == QUEEN){
            attackers |= Magics::getRookMoves(occupied, to) & rooks;
        }
    }
    return colorState != colorStart;
}