
#include "board.h"
#include "movegen.h"
namespace Sentinel{
    int Board::eval() {
        return whoPlay ? nnue.eval<WHITE>() : nnue.eval<BLACK>();
    }
}