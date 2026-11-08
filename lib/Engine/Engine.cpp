#include "Engine.h"
#include "Parser.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

InterfaceCommand::ExecuteType Engine::Execute(const std::string& command) {
    std::vector<std::string> tokens = Parser::ParseLine(command);

    if (tokens.size() == 0) return NullType{};

    std::string current_command = tokens[0];
    std::transform(current_command.begin(), current_command.end(), 
                    current_command.begin(), [](unsigned char c) {
                                                return std::toupper(c);
                                            });

    auto command_executor = cmd_reg_.GetCommand(current_command);

    if (!command_executor) {
        return ErrorReturnType{"unknown command '" + tokens[0] + "'"};
    }

    try {
        return command_executor->Execute(db_, tokens);
    } catch (const std::bad_alloc& e1) {
        db_.DeleteExpiredKeys(true);

        try {
            return command_executor->Execute(db_, tokens);
        } catch (const std::bad_alloc& e2) {
            return ErrorReturnType{"OOM command not allowed when used memory > 'maxmemory'"};
        }
    } catch (const std::exception& e) {
        return ErrorReturnType{e.what()};
    }
}

void Engine::Run() {
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line == "EXIT") return;

        try {
            auto result = Execute(line);

            std::visit(OverloadPattern {
                [](const NullType& t){ std::cout << "(nil)\n"; },
                [](const StatusReturnType& t){ std::cout << t.status << '\n'; },
                [](const IntegerReturnType& t){ std::cout << "(integer) " << t.data << '\n'; },
                [](const StringReturnType& t){ std::cout << "(string) " << t.data << '\n'; },
                [](const ArrayReturnType& t){
                    if (t.data.empty()) {
                        std::cout << "(empty array)" << std::endl;
                    } else {
                        for (size_t i = 0; i < t.data.size(); ++i) {
                            std::cout << i + 1 << ") " << t.data[i]<< '\n';
                        }
                    }
                },
                [](const SetReturnType& s){
                    if (s.data.empty()) {
                        std::cout << "(empty set)" << std::endl;
                    } else {
                        for (size_t i = 0; i < s.data.size(); ++i) {
                            std::cout << i + 1 << ") " << s.data[i] << '\n';
                        }
                    }
                },
                [](const GeoReturnType& grt) {
                    std::cout << "(geo index info)" << std::endl;
                    for (const auto& p : grt.points) {
                        std::cout << "- " << p.name << ": [" << p.lon << ", " << p.lat << "]\n";
                    }
                },
                [](const GeoArrayReturnType& t){
                    for (size_t i = 0; i < t.data.size(); ++i) {
                        std::cout << i + 1 << ") ";
                        if (t.data[i].has_value()) {
                            std::cout << "\n   1) \"" << t.data[i]->lon << "\"\n";
                            std::cout << "   2) \"" << t.data[i]->lat << "\"\n";
                        } else {
                            std::cout << "(nil)" << std::endl;
                        }
                    }
                },
                [](const ErrorReturnType& t){ std::cout << "(error) " << t.error_message << '\n'; }
            }, result);
        } catch (...) {
            std::cerr << "Unexpected error\n";
        }
    }
}
