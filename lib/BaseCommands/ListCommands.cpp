#include "ListCommands.h"
#include "../Engine/DataBase.h"
#include "../ValueObject/Value.h"
#include "../HelpingStructures.h"

#include <algorithm>

InterfaceCommand::ExecuteType LPUSHlist::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() <= 2) {
        return ErrorReturnType("wrong number of arguments for 'lpush' command");
    }

    auto res = db.GetPtr(tokens[1]);

    if (!res) {
        db.Set(tokens[1], ArrayReturnType());
        res = db.GetPtr(tokens[1]);
    }

    return std::visit(OverloadPattern {
        [&](ListType& s) -> ExecuteType { 
            for (int i = 2; i < tokens.size(); ++i) {
                s.PushFront(tokens[i]);
            }

            return IntegerReturnType(s.Len());
        },
        [&](auto&&) -> ExecuteType { 
            return ErrorReturnType("wrongtype operation against a key holding the wrong kind of value"); 
        }
    }, res->data_);
}

InterfaceCommand::ExecuteType RPUSHlist::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() <= 2) {
        return ErrorReturnType("wrong number of arguments for 'rpush' command");
    }

    auto res = db.GetPtr(tokens[1]);

    if (!res) {
        db.Set(tokens[1], ArrayReturnType());
        res = db.GetPtr(tokens[1]);
    }

    return std::visit(OverloadPattern {
        [&](ListType& s) -> ExecuteType { 
            for (int i = 2; i < tokens.size(); ++i) {
                s.PushBack(tokens[i]);
            }

            return IntegerReturnType(s.Len());
        },
        [&](auto&&) -> ExecuteType { 
            return ErrorReturnType("wrongtype operation against a key holding the wrong kind of value"); 
        }
    }, res->data_);
}


InterfaceCommand::ExecuteType RPOPlist::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 2 && tokens.size() != 3) {
        return ErrorReturnType("wrong number of arguments for 'rpop' command");
    }

    auto res = db.GetPtr(tokens[1]);

    if (!res) {
        return NullType();
    }

    return std::visit(OverloadPattern {
        [&](ListType& s) -> ExecuteType {
            if (s.Empty()) {
                return NullType();
            }

            if (tokens.size() == 2) {
                StringReturnType result = s.PopBack();
                if (s.Empty()) {
                    db.Delete(tokens[1]);
                }

                return StringReturnType(std::move(result));
            } else {
                long long ammount;
                try { 
                    ammount = std::stoll(tokens[2]); 
                } catch (...) { 
                    return ErrorReturnType("Must be an integer");
                }

                std::vector<std::string> result;

                while (!s.Empty() && ammount > 0) {
                    StringReturnType str = s.PopBack();
                    result.push_back(str.data);
                    --ammount;
                }

                if (s.Empty()) {
                    db.Delete(tokens[1]);
                }

                return ArrayReturnType(std::move(result));
            }
        },
        [&](auto&&) -> ExecuteType { 
            return ErrorReturnType("wrongtype operation against a key holding the wrong kind of value"); 
        }
    }, res->data_);
}

InterfaceCommand::ExecuteType LPOPlist::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 2 && tokens.size() != 3) {
        return ErrorReturnType("wrong number of arguments for 'rpop' command");
    }

    auto res = db.GetPtr(tokens[1]);

    if (!res) {
        return NullType();
    }

    return std::visit(OverloadPattern {
        [&](ListType& s) -> ExecuteType {
            if (s.Empty()) {
                return NullType();
            }

            if (tokens.size() == 2) {
                StringReturnType result = s.PopFront();
                if (s.Empty()) {
                    db.Delete(tokens[1]);
                }

                return StringReturnType(std::move(result));
            } else {
                long long ammount;
                try { 
                    ammount = std::stoll(tokens[2]); 
                } catch (...) { 
                    return ErrorReturnType("Must be an integer"); 
                }

                std::vector<std::string> result;

                while (!s.Empty() && ammount > 0) {
                    StringReturnType str = s.PopFront();
                    result.push_back(str.data);
                    --ammount;
                }

                if (s.Empty()) {
                    db.Delete(tokens[1]);
                }

                return ArrayReturnType(std::move(result));
            }
        },
        [&](auto&&) -> ExecuteType { 
            return ErrorReturnType("wrongtype operation against a key holding the wrong kind of value"); 
        }
    }, res->data_);
}

InterfaceCommand::ExecuteType LLENlist::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 2) {
        return ErrorReturnType("wrong number of arguments for 'llen' command");
    }

    auto res = db.GetPtr(tokens[1]);
    
    if (!res) {
        return IntegerReturnType(0);
    }

    return std::visit(OverloadPattern {
        [&](ListType& s) -> ExecuteType {
            return IntegerReturnType(s.Len());
        },
        [&](auto&&) -> ExecuteType {
            return ErrorReturnType("wrongtype operation against a key holding the wrong kind of value"); 
        }
    }, res->data_);
}

InterfaceCommand::ExecuteType LRANGElist::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 4) {
        return ErrorReturnType("wrong number of arguments for 'lrange' command");
    }

    auto res = db.GetPtr(tokens[1]);

    if (!res) {
        return NullType();
    }

    return std::visit(OverloadPattern {
        [&](ListType& s) -> ExecuteType {
            long long ind1 = std::stoi(tokens[2]);
            long long ind2 = std::stoi(tokens[3]);

            if (ind1 < 0) {
                ind1 += s.Len();
            }

            if (ind2 < 0) {
                ind2 += s.Len();
            }

            if (ind1 < 0) {
                ind1 = 0;
            }
            
            if (ind1 >= s.Len()) {
                return ArrayReturnType();
            }

            if (ind2 >= s.Len()) {
                ind2 = s.Len() - 1;
            }

            if (ind1 > ind2) {
                return ArrayReturnType();
            }

            std::vector<std::string> result;
            result.reserve(ind2 - ind1 + 1);

            for (long long i = ind1; i <= ind2; ++i) {
                result.push_back(std::string(s.data[i].c_str()));
            }

            return ArrayReturnType(std::move(result));
        },
        [&](auto&&) -> ExecuteType { 
            return ErrorReturnType("wrongtype operation against a key holding the wrong kind of value"); 
        }
    }, res->data_);
}

InterfaceCommand::ExecuteType LINDEXlist::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 3) {
        return ErrorReturnType("wrong number of arguments for 'lindex' command");
    }

    auto res = db.GetPtr(tokens[1]);

    if (!res) {
        return NullType();
    }

    return std::visit(OverloadPattern {
        [&](ListType& s) -> ExecuteType {
            long long ind;

            try { 
                ind = std::stoll(tokens[2]); 
            } catch(...) { 
                return ErrorReturnType("Must be an integer"); 
            }

            if (ind < 0) {
                ind += s.Len();
            }

            if (ind < 0 || ind >= s.Len()) {
                return NullType();
            }

            return StringReturnType(std::string(s.data[ind].c_str()));
        },
        [&](auto&&) -> ExecuteType { 
            return ErrorReturnType("wrongtype operation against a key holding the wrong kind of value"); 
        }
    }, res->data_);
}

InterfaceCommand::ExecuteType LSETlist::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 4) {
        return ErrorReturnType("wrong number of arguments for 'lindex' command");
    }

    auto res = db.GetPtr(tokens[1]);

    if (!res) {
        return ErrorReturnType("No key like this");
    }

    return std::visit(OverloadPattern {
        [&](ListType& s) -> ExecuteType {
            long long ind;
            try { 
                ind = std::stoll(tokens[2]); 
            } catch(...) { 
                return ErrorReturnType("Must be an integer"); 
            }

            if (ind < 0) {
                ind += s.Len();
            }

            if (ind < 0 || ind >= s.Len()) {
                return ErrorReturnType("Out of range");
            }

            s.SetIndex(ind, tokens[3]);

            return StatusReturnType("Ok");
        },
        [&](auto&&) -> ExecuteType { 
            return ErrorReturnType("wrongtype operation against a key holding the wrong kind of value"); 
        }
    }, res->data_);
}

InterfaceCommand::ExecuteType LINSERTlist::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() != 5) {
        return ErrorReturnType("wrong number of arguments for 'lindex' command");
    }

    auto res = db.GetPtr(tokens[1]);

    if (!res) {
        return IntegerReturnType();
    }

    return std::visit(OverloadPattern {
        [&](ListType& s) -> ExecuteType {
            std::string qualifire = tokens[2];
            std::transform(qualifire.begin(), qualifire.end(), qualifire.begin(), [](unsigned char c) {
                return std::toupper(c);
            });
            bool before = (qualifire == "BEFORE");

            if (!before && qualifire != "AFTER") {
                return ErrorReturnType("Qualifire must be BEFORE/AFTER");
            }

            auto it = std::find(s.data.begin(), s.data.end(), tokens[3].c_str());
            if (it == s.data.end()) {
                return IntegerReturnType(-1);
            }

            if (before) {
                s.data.emplace(it, tokens[4].c_str());
            } else {
                s.data.emplace(std::next(it), tokens[4].c_str());
            }

             return IntegerReturnType(s.Len());
        },
        [&](auto&&) -> ExecuteType { 
            return ErrorReturnType("wrongtype operation against a key holding the wrong kind of value"); 
        }
    }, res->data_);
}
