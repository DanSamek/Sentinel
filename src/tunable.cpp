#include <iostream>
#include <cassert>

#include "tunable.h"

namespace Sentinel{
    void Tunable::print() {
        std::cout << "option name " << name << " type spin default " << def
                  << " min " << min << " max " << max << std::endl;
    }

    // For a weather factory.
    void Tunable::printJSON() {
        std::cout << "    \"" << name << "\": {\n";
        std::cout << "        \"value\": " << def << ",\n";
        std::cout << "        \"min_value\": " << min << ",\n";
        std::cout << "        \"max_value\": " << max << ",\n";
        std::cout << "        \"step\": " << "x" << "\n";
        std::cout << "    },\n";
    }

    void Tunable::setValue(int value) {
        assert(value >= min && value <= max);
        current = value;
    }

    std::map<std::string, Tunable*>& Tunable::getRegistry() {
        static std::map<std::string, Tunable*> tunableRegistry;
        return tunableRegistry;
    }

    void Tunable::registerTunable() {
        getRegistry()[name] = this;
    }

    Tunable* Tunable::getByName(const std::string& name) {
        auto it = getRegistry().find(name);
        return (it != getRegistry().end()) ? it->second : nullptr;
    }
}