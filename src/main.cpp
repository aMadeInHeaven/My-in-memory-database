#include <string>
#include <algorithm>

#include "Engine/Engine.h"


size_t ParseMemoryLimit(std::string arg) {
    if (arg.empty()) {
        return SIZE_MAX;
    }
    if (arg == "0") {
        return SIZE_MAX;
    }

    std::transform(arg.begin(), arg.end(), arg.begin(), [](unsigned char c) { 
        return std::tolower(c); 
    });

    size_t multiplier = 1;
    std::string numeric_part = arg;

    if (arg.size() > 2) {
        std::string unit = arg.substr(arg.size() - 2);
        if (unit == "kb") {
            multiplier = 1024;
            numeric_part = arg.substr(0, arg.size() - 2);
        } else if (unit == "mb") {
            multiplier = 1024ULL * 1024;
            numeric_part = arg.substr(0, arg.size() - 2);
        } else if (unit == "gb") {
            multiplier = 1024ULL * 1024 * 1024;
            numeric_part = arg.substr(0, arg.size() - 2);
        }
    }

    if (multiplier == 1 && arg.back() == 'b') {
        numeric_part = arg.substr(0, arg.size() - 1);
    }

    try {
        return std::stoull(numeric_part) * multiplier;
    } catch (...) {
        return SIZE_MAX; 
    }
}

int main(int argc, char** argv) {
    size_t limit = SIZE_MAX;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--maxmemory" && i + 1 < argc) {
            limit = ParseMemoryLimit(argv[++i]);
        }
    }

    if (limit == 0) {
        limit = SIZE_MAX;
    }

    Engine engine(limit);

    engine.Run();
    return 0;
}