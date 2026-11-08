#include <vector>
#include <string>

#include "StringCommands.h"
#include "../Engine/DataBase.h"
#include "../ValueObject/Value.h"
#include "../HelpingStructures.h"


InterfaceCommand::ExecuteType SETstring::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 3) {
        return ErrorReturnType("wrong number of arguments for 'set' command");
    }

    try {
        db.Set(tokens[1], StringReturnType(tokens[2]));
        return StatusReturnType("OK");
    } catch (const std::bad_alloc&) {
        return ErrorReturnType("OOM command not allowed");
    }
}

InterfaceCommand::ExecuteType GETstring::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 2) {
        return ErrorReturnType("wrong number of arguments for 'get' command");
    }

    auto res = db.Get(tokens[1]);

    return std::visit(OverloadPattern {
        [](const StringReturnType& s) -> ExecuteType { 
            return s; 
        },
        [](const NullType& n) -> ExecuteType { 
            return n; 
        },
        [](auto&&) -> ExecuteType { 
            return ErrorReturnType("wrongtype Operation against a key holding the wrong kind of value"); 
        }
    }, std::as_const(res));
}

InterfaceCommand::ExecuteType STRLENstring::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 2) {
        return ErrorReturnType("wrong number of arguments for 'strlen' command");
    }

    auto* res = db.GetPtr(tokens[1]);

    if (!res) {
        return NullType();
    }

    return std::visit(OverloadPattern {
        [](StringType& s) -> ExecuteType { 
            return IntegerReturnType(static_cast<long long>(s.data.size())); 
        },
        [](NoneType&) -> ExecuteType { 
            return IntegerReturnType(0); 
        },
        [](auto&&) -> ExecuteType { 
            return ErrorReturnType("wrongtype Operation against a key holding the wrong kind of value"); 
        }
    }, res->data_);
}

InterfaceCommand::ExecuteType APPENDstring::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 3) {
        return ErrorReturnType("wrong number of arguments for 'append' command");
    }

    auto res = db.GetPtr(tokens[1]);

    if (!res) {
        db.Set(tokens[1], StringReturnType());
        res = db.GetPtr(tokens[1]);
    }

    return std::visit(OverloadPattern {
        [&](StringType& s) -> ExecuteType {
            try {
                s.Append(tokens[2]);
                return IntegerReturnType(static_cast<long long>(s.data.size()));
            } catch (const std::bad_alloc&) { 
                return ErrorReturnType("OOM"); 
            }
        },
        [&](NoneType&) -> ExecuteType {
            try {
                db.Set(tokens[1], StringReturnType(tokens[2]));
                return IntegerReturnType(static_cast<long long>(tokens[2].size()));
            } catch (const std::bad_alloc&) { 
                return ErrorReturnType("OOM"); 
            }
        },
        [](auto&& a) -> ExecuteType { 
            std::cout << typeid(a).name() << '\n';
            return ErrorReturnType("wrongtype Operation against a key holding the wrong kind of value"); 
        }
    }, res->data_);
}

InterfaceCommand::ExecuteType EXPIREstring::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 3) {
        return ErrorReturnType("wrong number of arguments for 'expire' command");
    }

    if (!db.IsKeyExist(tokens[1])) {
        return IntegerReturnType(0);
    }

    try {
        long long seconds = std::stoll(tokens[2]);
        db.SetTTL(tokens[1], seconds);
        return IntegerReturnType(1);
    } catch (...) {
        return ErrorReturnType("value is not an integer or out of range");
    }
}

InterfaceCommand::ExecuteType TTLstring::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 2) {
        return ErrorReturnType("wrong number of arguments for 'ttl' command");
    }

    if (!db.IsKeyExist(tokens[1])) {
        return IntegerReturnType(-2);
    }

    return IntegerReturnType(db.TimeLeftUntilExpired(tokens[1])); 
}
