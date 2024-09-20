#include "neuralnetwork.h"
#include "fstream"
#include "algorithm"
#include "chrono"
#include "boardEnums.h"


template<typename T>
T readNumber(std::ifstream& stream){
    T value;
    stream.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
}

inline int getIndex(PIECE_COLOR perspective, PIECE_COLOR color, PIECE_TYPE piece, int square) {
    auto colorIndex = (perspective == color) ? 0 : 1;
    auto pieceIndex = (int)piece;
    auto squareIndex = (perspective == PIECE_COLOR::WHITE) ? square ^ 56 : square;

    auto result = (colorIndex * 6 + pieceIndex) * 64 + squareIndex;
    return result;
}

template<PIECE_COLOR us, PIECE_COLOR opp>
std::vector<int> getInputLayer(const Board& board){
    std::vector<int> indexes;
    uint64_t all = 0ULL;
    for(int j = 0; j <= PIECE_TYPE::KING; j++){
        all |= board.colorBBS(us)[j];
        all |= board.colorBBS(opp)[j];
    }

    while(all){
        auto square = bit_ops::bitScanForwardPopLsb(all);
        auto piece = board.getPieceTypeFromSQ(square, board.colorBBS(us));
        if(piece.second){
            indexes.push_back(getIndex(us, us, piece.first, square));
            continue;
        }
        piece = board.getPieceTypeFromSQ(square, board.colorBBS(opp));
        if(piece.second){
            indexes.push_back(getIndex(us, opp, piece.first, square));
        }
    }

    return indexes;
}

std::array<int, HIDDEN_LAYER_SIZE> NeuralNetwork::forwardInputLayer(const std::vector<int> & inputLayer){
    std::array<int, HIDDEN_LAYER_SIZE> result = {};

    for(auto i : inputLayer){
        for(int x = 0; x < HIDDEN_LAYER_SIZE; x++){
            result[x] += INPUT_LAYER[i][x];
        }
    }

    for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
        result[i] += INPUT_LAYER_BIASES[i];
    }

    return result;
}


int NeuralNetwork::eval(const Board& board) {
    auto usInputLayer = board.whoPlay ? getInputLayer<PIECE_COLOR::WHITE, PIECE_COLOR::BLACK>(board) : getInputLayer<PIECE_COLOR::BLACK, PIECE_COLOR::WHITE>(board);
    auto oppInputLayer = board.whoPlay ? getInputLayer<PIECE_COLOR::BLACK, PIECE_COLOR::WHITE>(board) : getInputLayer<PIECE_COLOR::WHITE, PIECE_COLOR::BLACK>(board);

    auto usForward = forwardInputLayer(usInputLayer);
    auto oppForward = forwardInputLayer(oppInputLayer);

    auto result = HIDDEN_LAYER_BIASES[0];

    for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
        result += crelu(usForward[i]) * HIDDEN_LAYER_WEIGHTS[i];
        result += crelu(oppForward[i]) * HIDDEN_LAYER_WEIGHTS[i + HIDDEN_LAYER_SIZE];
    }

    result *= SCALE;
    result /= QA * QB;
    return result;
}

void NeuralNetwork::load() {
    std::ifstream stream(NET_PATH, std::ios::binary);

    if(!stream.is_open()){
        throw "File cannot be opened";
    }

    for(int i = 0; i < INPUT_LAYER_SIZE; i++){
        for(int x = 0; x < HIDDEN_LAYER_SIZE; x++){
            INPUT_LAYER[i][x] = readNumber<int16_t>(stream);
        }
    }
    assert(!stream.eof());
    for(int i = 0; i < HIDDEN_LAYER_SIZE; i++){
        INPUT_LAYER_BIASES[i] = readNumber<int16_t>(stream);
    }

    assert(!stream.eof());
    for(int i = 0; i < HIDDEN_LAYER_SIZE * 2; i++){
        HIDDEN_LAYER_WEIGHTS[i] = readNumber<int16_t>(stream);
    }
    assert(!stream.eof());

    HIDDEN_LAYER_BIASES[0] = readNumber<int16_t>(stream);
}


void NeuralNetwork::test() {
    load();

    Board b;
    auto testSuite = [&](std::string fen) ->void{
        int result;
        b.loadFEN(fen);

        for(int i = 0; i < 250'000; i++){
            result = eval(b);
            if((i + 1) % 250'000 == 0){
                std::cout << result << std::endl;
            }
        }
    };

    auto start = std::chrono::high_resolution_clock::now();
    testSuite("r1bqkbnr/ppp1pppp/2n5/3p4/3P4/1BN2N2/PPP1PPPP/R1BQ1RK1 w kq - 0 1");
    testSuite("r2qkb1r/ppp1pppp/2n2n2/3p1b2/3P4/1BN2N2/PPP1PPPP/R1BQ1RK1 w kq - 0 1");
    testSuite("rnbqkbnr/pppppppp/8/3P4/4P3/1BN2N2/PPP1QPPP/R1B2RK1 w kq - 0 1");
    testSuite("r1bqkbnr/pppp1ppp/2n5/4p3/4P3/3P4/PPP2PPP/RNBQKBNR w KQkq - 0 1");
    testSuite("1kr2b1r/ppp2ppp/2np1n2/4pb2/8/8/PPPPPPPP/RNB1KBNR w KQk - 1 1");

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    double microseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    std::cout << "Elapsed time: " << microseconds << " ms" << std::endl;
    auto testSuiteClassic = [&](std::string fen) ->void{
        int result;
        b.loadFEN(fen);

        for(int i = 0; i < 250'000; i++){
            result = b.eval();
            if((i + 1) % 250'000 == 0){
                std::cout << result << std::endl;
            }
        }
    };

    start = std::chrono::high_resolution_clock::now();
    testSuiteClassic("r1bqkbnr/ppp1pppp/2n5/3p4/3P4/1BN2N2/PPP1PPPP/R1BQ1RK1 w kq - 0 1");
    testSuiteClassic("r2qkb1r/ppp1pppp/2n2n2/3p1b2/3P4/1BN2N2/PPP1PPPP/R1BQ1RK1 w kq - 0 1");
    testSuiteClassic("rnbqkbnr/pppppppp/8/3P4/4P3/1BN2N2/PPP1QPPP/R1B2RK1 w kq - 0 1");
    testSuiteClassic("r1bqkbnr/pppp1ppp/2n5/4p3/4P3/3P4/PPP2PPP/RNBQKBNR w KQkq - 0 1");
    testSuiteClassic("1kr2b1r/ppp2ppp/2np1n2/4pb2/8/8/PPPPPPPP/RNB1KBNR w KQk - 1 1");

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;

    microseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    std::cout << "Elapsed time: " << microseconds << " ms" << std::endl;
}