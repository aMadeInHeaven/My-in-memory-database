#include "Parser.h"

std::vector<std::string> Parser::ParseLine(const std::string& line) {
    std::vector<std::string> tokens;

    std::stringstream stream(line);
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}
