#include "tests/TestRunner.cpp"
#include "nnue/datagen.h"
#include "uci.h"
#include "development.h"

int main(){
    if(RUN_DATAGEN){
        Datagen().run();
    }
    else if(RUN_TESTS){
        TestRunner::run();

    }else{
        UCI::loop();
    }

    return 0;
}