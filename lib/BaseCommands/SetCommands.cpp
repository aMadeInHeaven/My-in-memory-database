#include "SetCommands.h"
#include "../Engine/DataBase.h"
#include "../ValueObject/Value.h"
#include "../HelpingStructures.h"

#include <vector>
#include <string>
#include <variant>
#include <algorithm>
#include <iterator>
#include <set>

std::set<std::string> SetFromArray(DataBase& db, const std::string& key, bool& error) {
    ValueContainer* container = db.GetPtr(key);

    if (!container) {
        return {};
    }

    auto* set_ptr = std::get_if<SetType>(&container->data_);

    if (!set_ptr) {
        error = true;
        return {};
    }

    std::set<std::string> result;

    for (const auto& elem : set_ptr->data) {
        result.insert(std::string(elem.c_str()));
    }

    return result;
}

InterfaceCommand::ExecuteType SADDset::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 3) {
        return ErrorReturnType{"ERR wrong number of arguments for 'sadd' command"};
    }

    auto res = db.GetPtr(tokens[1]);

    if (!res) {
        db.Set(tokens[1], SetReturnType());
        res = db.GetPtr(tokens[1]);
    }

    return std::visit(OverloadPattern {
        [&](SetType& s) -> ExecuteType {
            long long added = 0;
            size_t size_before = s.data.size();

            for (size_t i = 2; i < tokens.size(); ++i) {
                s.Add(tokens[i]);
            }

            added = s.data.size() - size_before;
            return IntegerReturnType(added);
        },
        [](auto&&) -> ExecuteType { 
            return ErrorReturnType{"wrongtype operation against a key holding the wrong kind of value"}; 
        }
    }, res->data_);
}

InterfaceCommand::ExecuteType SREMset::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 3) {
        return ErrorReturnType{"ERR wrong number of arguments for 'srem' command"};
    }

    auto res = db.GetPtr(tokens[1]);

    if (!res) {
        return IntegerReturnType();
    }

    return std::visit(OverloadPattern {
        [&](SetType& s) -> ExecuteType {
            long long removed = 0;

            for (size_t i = 2; i < tokens.size(); ++i) {
                size_t size_before = s.data.size();
                s.Remove(tokens[i]);
                if (s.data.size() < size_before) {
                    ++removed;
                }
            }

            if (s.data.empty()) {
                db.Delete(tokens[1]);
            }

            return IntegerReturnType(removed);
        },
        [](auto&&) -> ExecuteType { return ErrorReturnType{"wrongtype operation against a key holding the wrong kind of value"}; }
    }, res->data_);
}

InterfaceCommand::ExecuteType SISMEMBERset::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 3) {
        return ErrorReturnType{"ERR wrong number of arguments for 'sismember' command"};
    }

    auto res = db.GetPtr(tokens[1]);

    if (!res) {
        return IntegerReturnType();
    }

    return std::visit(OverloadPattern {
        [&](SetType& s) -> ExecuteType {
            bool found = (s.data.find(DBstring(tokens[2].c_str())) != s.data.end());

            return IntegerReturnType(found ? 1 : 0);
        },
        [](auto&&) -> ExecuteType { 
            return ErrorReturnType{"wrongtype operation against a key holding the wrong kind of value"}; 
        }
    }, res->data_);
}

InterfaceCommand::ExecuteType SMEMBERSset::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 2) {
        return ErrorReturnType{"ERR wrong number of arguments for 'smembers' command"};
    }

    ValueContainer* container = db.GetPtr(tokens[1]);
    if (!container) {
        return NullType();
    }

    if (!std::holds_alternative<SetType>(container->data_)) {
        return ErrorReturnType{"wrongtype operation against a key holding the wrong kind of value"};
    }

    return db.Get(tokens[1]);
}

InterfaceCommand::ExecuteType SCARDset::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 2) {
        return ErrorReturnType{"ERR wrong number of arguments for 'scard' command"};
    }

    auto res = db.GetPtr(tokens[1]);

    if (!res) {
        return IntegerReturnType();
    }

    return std::visit(OverloadPattern {
        [&](SetType& s) -> ExecuteType { 
            return IntegerReturnType(s.Len()); 
        },
        [](auto&&) -> ExecuteType { 
            return ErrorReturnType{"wrongtype operation against a key holding the wrong kind of value"}; 
        }
    }, res->data_);
}

InterfaceCommand::ExecuteType SINTERset::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        return ErrorReturnType{"ERR wrong number of arguments for 'sinter' command"};
    }

    bool error = false;
    std::set<std::string> result = SetFromArray(db, tokens[1], error);

    if (error) {
        return ErrorReturnType{"wrongtype operation against a key holding the wrong kind of value"};
    }

    for (size_t i = 2; i < tokens.size(); ++i) {
        std::set<std::string> new_one = SetFromArray(db, tokens[i], error);

        if (error) {
            return ErrorReturnType{"wrongtype operation against a key holding the wrong kind of value"};
        }
        
        std::set<std::string> intersection;
        std::set_intersection(result.begin(), result.end(), new_one.begin(), new_one.end(), std::inserter(intersection, intersection.begin()));
        result = std::move(intersection);

        if (result.empty()) {
            break;
        }
    }

    return ArrayReturnType(std::vector<std::string>(result.begin(), result.end()));
}

InterfaceCommand::ExecuteType SUNIONset::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        return ErrorReturnType{"ERR wrong number of arguments for 'sunion' command"};
    }

    std::set<std::string> result;
    bool error = false;

    for (size_t i = 1; i < tokens.size(); ++i) {
        std::set<std::string> new_one = SetFromArray(db, tokens[i], error);

        if (error) {
            return ErrorReturnType{"wrongtype operation against a key holding the wrong kind of value"};
        }

        result.insert(new_one.begin(), new_one.end());
    }

    return ArrayReturnType(std::vector<std::string>(result.begin(), result.end()));
}

InterfaceCommand::ExecuteType SDIFFset::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        return ErrorReturnType{"ERR wrong number of arguments for 'sdiff' command"};
    }

    bool error = false;
    std::set<std::string> result = SetFromArray(db, tokens[1], error);

    if (error) {
        return ErrorReturnType{"wrongtype operation against a key holding the wrong kind of value"};
    }

    for (size_t i = 2; i < tokens.size(); ++i) {
        std::set<std::string> new_one = SetFromArray(db, tokens[i], error);

        if (error) {
            return ErrorReturnType{"wrongtype operation against a key holding the wrong kind of value"};
        }

        for (const auto& item : new_one) {
            result.erase(item);
        }
    }

    return ArrayReturnType(std::vector<std::string>(result.begin(), result.end()));
}

InterfaceCommand::ExecuteType SMOVEset::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 4) {
        return ErrorReturnType{"ERR wrong number of arguments for 'smove' command"};
    }
    
    auto source_ptr = db.GetPtr(tokens[1]);
    if (!source_ptr) {
        return IntegerReturnType();
    }

    if (!std::holds_alternative<SetType>(source_ptr->data_)) {
        return ErrorReturnType{"WRONGTYPE Operation against a key holding the wrong kind of value"};
    }

    auto& source = std::get<SetType>(source_ptr->data_);
    DBstring member(tokens[3].c_str());
    
    if (source.data.find(member) == source.data.end()) {
        return IntegerReturnType();
    }

    auto dest_ptr = db.GetPtr(tokens[2]);

    if (dest_ptr && !std::holds_alternative<SetType>(dest_ptr->data_)) {
        return ErrorReturnType{"WRONGTYPE Operation against a key holding the wrong kind of value"};
    }
    
    if (!dest_ptr) {
        db.Set(tokens[2], SetReturnType());
        dest_ptr = db.GetPtr(tokens[2]);
    }

    source.Remove(tokens[3]);
    if (source.data.empty()) {
        db.Delete(tokens[1]);
    }

    std::get<SetType>(dest_ptr->data_).Add(tokens[3]);

    return IntegerReturnType(1);
}
