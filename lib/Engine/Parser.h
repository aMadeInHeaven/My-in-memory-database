#pragma once

#include <vector>
#include <string>
#include <sstream>

struct Parser {
    static std::vector<std::string> ParseLine(const std::string& line);
};