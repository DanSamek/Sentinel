#ifndef SENTINEL_TUNABLE_H
#define SENTINEL_TUNABLE_H

#include <string>
#include <map>

namespace Sentinel{

    /***
     * Struct for a tunable search params.
     * --> https://github.com/jnlt3/weather-factory
     */
    struct Tunable{
        std::string name;
        int min, max, def;
        int current;

        Tunable(std::string name, int min, int max, int def) : name(name), min(min), max(max), def(def), current(def) {
            registerTunable();
        }

        void print();
        void printJSON();
        void setValue(int value);

        static Tunable* getByName(const std::string& name);

        static std::map<std::string, Tunable*>& getRegistry();

        operator int() const {
            return current;
        }

    private:
        void registerTunable();
    };
}


#endif //SENTINEL_TUNABLE_H
