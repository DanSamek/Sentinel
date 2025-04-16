#include "tests/TestRunner.cpp"
#include "nnue/datagen.h"
#include "uci.h"
#include "development.h"
#include "bench.cpp"

int main(int argc, char* argv[]){
#if RUN_DATAGEN
        Datagen().run();
#elif RUN_TESTS
        TestRunner::run();
#else
        if (argc <= 1){
            UCI::loop();
        }
        else if(strcmp(argv[1], "bench") == 0){
            Bench::run();
        }
#endif
    return 0;
}