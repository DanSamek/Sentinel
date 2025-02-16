#include "tests/TestRunner.cpp"
#include "nnue/datagen.h"
#include "uci.h"
#include "development.h"

int main(){
#if RUN_DATAGEN
        Datagen().run();
#elif RUN_TESTS
        TestRunner::run();
#else
        UCI::loop();
#endif
    return 0;
}