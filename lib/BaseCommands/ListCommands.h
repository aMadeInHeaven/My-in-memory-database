#pragma once

#include <vector>
#include <string>

#include "../CommandCore/Command.h"
#include "../ReturnTypes/ReturnTypes.h"

class LPUSHlist : public InterfaceCommand {
public:
    LPUSHlist() : InterfaceCommand("LPUSH") {}
    
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};

class RPUSHlist : public InterfaceCommand {
public:
    RPUSHlist() : InterfaceCommand("RPUSH") {}

    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};

class LPOPlist : public InterfaceCommand {
public:
    LPOPlist() : InterfaceCommand("LPOP") {}

    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};

class RPOPlist : public InterfaceCommand {
public:
    RPOPlist() : InterfaceCommand("RPOP") {}
    
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};

class LLENlist : public InterfaceCommand {
public:
    LLENlist() : InterfaceCommand("LLEN") {}

    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};

class LRANGElist : public InterfaceCommand {
public:
    LRANGElist() : InterfaceCommand("LRANGE") {}

    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};

class LINDEXlist : public InterfaceCommand {
public:
    LINDEXlist() : InterfaceCommand("LINDEX") {}

    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};

class LSETlist : public InterfaceCommand {
public:
    LSETlist() : InterfaceCommand("LSET") {}

    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};

class LINSERTlist : public InterfaceCommand {
public:
    LINSERTlist() : InterfaceCommand("LINSERT") {}

    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};
