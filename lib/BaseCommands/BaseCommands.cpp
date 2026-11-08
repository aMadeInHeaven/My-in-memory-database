#include "BaseCommands.h"
#include "../Engine/DataBase.h"
#include "../ValueObject/Value.h"
#include "../HelpingStructures.h"

#include <vector>
#include <string>
#include <string_view>
#include <algorithm>
#include <variant>


InterfaceCommand::ExecuteType TYPEcmd::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 2) {
        return ErrorReturnType("wrong number of arguments for 'type' command");
    }

    return StatusReturnType(db.GetTypeName(tokens[1]));
}

InterfaceCommand::ExecuteType DELcmd::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        return ErrorReturnType("wrong number of arguments for 'del' command");
    }

    long long deleted = 0;
    for (size_t i = 1; i < tokens.size(); ++i) {
        if (db.IsKeyExist(tokens[i])) {
            db.Delete(tokens[i]);
            ++deleted;
        }
    }
    return IntegerReturnType(deleted);
}

InterfaceCommand::ExecuteType EXISTScmd::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        return ErrorReturnType("wrong number of arguments for 'exists' command");
    }

    long long counter = 0;
    for (size_t i = 1; i < tokens.size(); ++i) {
        if (db.IsKeyExist(tokens[i])) {
            ++counter;
        }
    }
    return IntegerReturnType(counter);
}

InterfaceCommand::ExecuteType KEYScmd::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        return ErrorReturnType("wrong number of arguments for 'keys' command");
    }

    const std::string& pattern = tokens[1];
    std::vector<std::string> all_keys = db.GetAllKeys();
    std::vector<std::string> matched_keys;
    
    auto easy_regex = [](auto self, std::string_view pattern, std::string_view key) -> bool {
        if (pattern.empty()) {
            return key.empty();
        }

        if (pattern[0] == '*') {
            for (size_t i = 0; i <= key.size(); ++i) {
                if (self(self, pattern.substr(1), key.substr(i))) {
                    return true;
                }
            }
            return false;
        }

        if (key.empty()) {
            return false;
        }

        if (pattern[0] == '?') {
            return self(self, pattern.substr(1), key.substr(1));
        }

        if (pattern[0] == '[') {
            size_t close = pattern.find(']', 1);
            if (close == std::string_view::npos) {
                return false;
            }
            
            std::string_view chars = pattern.substr(1, close - 1);

            if (chars.find(key[0]) == std::string_view::npos) {
                return false;
            }
            
            return self(self, pattern.substr(close + 1), key.substr(1));
        }

        if (pattern[0] != key[0]) {
            return false;
        }
        
        return self(self, pattern.substr(1), key.substr(1));
    };

    for (const auto& key : all_keys) {
        if (easy_regex(easy_regex, pattern, key)) {
            matched_keys.push_back(key);
        }
    }

    return ArrayReturnType(std::move(matched_keys));
}

InterfaceCommand::ExecuteType FLUSHDBcmd::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 1) {
        return ErrorReturnType("wrong number of arguments for 'flushdb' command");
    }

    db.Flush();
    return StatusReturnType("OK");
}

InterfaceCommand::ExecuteType CONFIGcmd::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 3) {
        return ErrorReturnType("wrong number of arguments for 'config' command");
    }
    
    std::string sub = tokens[1];
    std::transform(sub.begin(), sub.end(), sub.begin(), [](unsigned char c) {
        return std::toupper(c);
    });

    if (sub == "GET") {
        if (tokens[2] == "maxmemory") {
            return ArrayReturnType({"maxmemory", std::to_string(db.GetMaxMemory())});
        }
    } else if (sub == "SET") {
        if (tokens.size() < 4) {
            return ErrorReturnType("wrong number of arguments");
        }

        if (tokens[2] == "maxmemory") {
            try {
                unsigned long long val = std::stoull(tokens[3]);

                if (val == 0) {
                    db.SetMaxMemory(SIZE_MAX);
                } else {
                    db.SetMaxMemory(static_cast<size_t>(val));
                }
                
                return StatusReturnType("OK");
            } catch (...) {
                return ErrorReturnType("value must be an integer");
            }
        }
    }

    return ErrorReturnType("unknown CONFIG parameter");
}

InterfaceCommand::ExecuteType DBSIZEcmd::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 1) {
        return ErrorReturnType("wrong number of arguments for 'dbsize' command");
    }

    return IntegerReturnType(db.GetKeysAmmount());
}

InterfaceCommand::ExecuteType MEMORYUSAGEcmd::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 3) {
        return ErrorReturnType("wrong number of arguments for 'memoryusage' command");
    }

    std::string sub = tokens[1];
    std::transform(sub.begin(), sub.end(), sub.begin(), [](unsigned char c) {
        return std::toupper(c);
    });

    if (sub == "USAGE") {
        ValueContainer* vc = db.GetPtr(tokens[2]);
        if (!vc) {
            return NullType();
        }

        return IntegerReturnType(vc->MemoryUsed());
    }
    
    return ErrorReturnType("unknown MEMORYUSAGE parameter");
}
