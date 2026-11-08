#pragma once

#include "../CommandCore/Command.h"


class SADDset : public InterfaceCommand { 
public: 
    SADDset() : InterfaceCommand("SADD") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class SREMset : public InterfaceCommand { 
public: 
    SREMset() : InterfaceCommand("SREM") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class SISMEMBERset : public InterfaceCommand { 
public: 
    SISMEMBERset() : InterfaceCommand("SISMEMBER") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class SMEMBERSset : public InterfaceCommand { 
public: 
    SMEMBERSset() : InterfaceCommand("SMEMBERS") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class SCARDset : public InterfaceCommand { 
public: 
    SCARDset() : InterfaceCommand("SCARD") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class SUNIONset : public InterfaceCommand { 
public: 
    SUNIONset() : InterfaceCommand("SUNION") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class SINTERset : public InterfaceCommand { 
public: 
    SINTERset() : InterfaceCommand("SINTER") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class SDIFFset : public InterfaceCommand { 
public: 
    SDIFFset() : InterfaceCommand("SDIFF") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class SMOVEset : public InterfaceCommand { 
public: 
    SMOVEset() : InterfaceCommand("SMOVE") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};
