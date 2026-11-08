#pragma once

#include <vector>
#include <string>

#include "../CommandCore/Command.h"
#include "../ReturnTypes/ReturnTypes.h"

class SETstring : public InterfaceCommand {
public:
    SETstring() : InterfaceCommand("SET") {}
    
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};

class GETstring : public InterfaceCommand {
public:
    GETstring() : InterfaceCommand("GET") {}

    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};

class STRLENstring : public InterfaceCommand {
public:
    STRLENstring() : InterfaceCommand("STRLEN") {}

    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};

class APPENDstring : public InterfaceCommand {
public:
    APPENDstring() : InterfaceCommand("APPEND") {}
    
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};

class EXPIREstring : public InterfaceCommand {
public:
    EXPIREstring() : InterfaceCommand("EXPIRE") {}

    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};

class TTLstring : public InterfaceCommand {
public:
    TTLstring() : InterfaceCommand("TTL") {}

    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override;
};
