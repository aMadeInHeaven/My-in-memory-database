#pragma once

#include "../ReturnTypes/ReturnTypes.h"

#include <vector>
#include <string>

class DataBase;

class InterfaceCommand {
public:
    using ExecuteType = ReturnTypesVariant;

    InterfaceCommand(const std::string& name) : name_(name) {}
    virtual ~InterfaceCommand() = default;

    virtual ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) = 0;
private:
    std::string name_;
};
