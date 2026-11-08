#pragma once

#include "../CommandCore/Command.h"

class TYPEcmd : public InterfaceCommand { 
public: 
    TYPEcmd() : InterfaceCommand("TYPE") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class DELcmd : public InterfaceCommand { 
public: 
    DELcmd() : InterfaceCommand("DEL") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class EXISTScmd : public InterfaceCommand { 
public: 
    EXISTScmd() : InterfaceCommand("EXISTS") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class KEYScmd : public InterfaceCommand { 
public: 
    KEYScmd() : InterfaceCommand("KEYS") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class FLUSHDBcmd : public InterfaceCommand { 
public: 
    FLUSHDBcmd() : InterfaceCommand("FLUSHDB") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class DBSIZEcmd : public InterfaceCommand { 
public: 
    DBSIZEcmd() : InterfaceCommand("DBSIZE") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class CONFIGcmd : public InterfaceCommand { 
public: 
    CONFIGcmd() : InterfaceCommand("CONFIG") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class MEMORYUSAGEcmd : public InterfaceCommand { 
public: 
    MEMORYUSAGEcmd() : InterfaceCommand("MEMORY") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};
