#pragma once

#include "../CommandCore/Command.h"


class GEOADDgeo : public InterfaceCommand { 
public: 
    GEOADDgeo() : InterfaceCommand("GEOADD") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class GEOPOSgeo : public InterfaceCommand { 
public: 
    GEOPOSgeo() : InterfaceCommand("GEOPOS") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class GEODISTgeo : public InterfaceCommand { 
public: 
    GEODISTgeo() : InterfaceCommand("GEODIST") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class GEOSEARCHgeo : public InterfaceCommand { 
public: 
    GEOSEARCHgeo() : InterfaceCommand("GEOSEARCH") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};

class GEOSEARCHSTOREgeo : public InterfaceCommand { 
public: 
    GEOSEARCHSTOREgeo() : InterfaceCommand("GEOSEARCHSTORE") {} 
    ExecuteType Execute(DataBase& db, const std::vector<std::string>& tokens) override; 
};
