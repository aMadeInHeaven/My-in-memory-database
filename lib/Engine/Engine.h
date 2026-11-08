#pragma once

#include "DataBase.h"
#include "CommandRegister.h"
#include "../CommandCore/Command.h"
#include "../ReturnTypes/ReturnTypes.h"
#include "../HelpingStructures.h"

#include "../BaseCommands/StringCommands.h"
#include "../BaseCommands/ListCommands.h"
#include "../BaseCommands/BaseCommands.h"
#include "../BaseCommands/SetCommands.h"
#include "../BaseCommands/GeoCommands.h"

#include <vector>
#include <string>
#include <iostream>

class Engine {
public:
    Engine(size_t db_size = SIZE_MAX) {
        cmd_reg_.RegisterCommand<GETstring>("GET");
        cmd_reg_.RegisterCommand<SETstring>("SET");
        cmd_reg_.RegisterCommand<STRLENstring>("STRLEN");
        cmd_reg_.RegisterCommand<APPENDstring>("APPEND");
        cmd_reg_.RegisterCommand<EXPIREstring>("EXPIRE");
        cmd_reg_.RegisterCommand<TTLstring>("TTL");

        cmd_reg_.RegisterCommand<LPUSHlist>("LPUSH");
        cmd_reg_.RegisterCommand<RPUSHlist>("RPUSH");
        cmd_reg_.RegisterCommand<LPOPlist>("LPOP");
        cmd_reg_.RegisterCommand<RPOPlist>("RPOP");
        cmd_reg_.RegisterCommand<LLENlist>("LLEN");
        cmd_reg_.RegisterCommand<LRANGElist>("LRANGE");
        cmd_reg_.RegisterCommand<LINDEXlist>("LINDEX");
        cmd_reg_.RegisterCommand<LSETlist>("LSET");
        cmd_reg_.RegisterCommand<LINSERTlist>("LINSERT");

        cmd_reg_.RegisterCommand<TYPEcmd>("TYPE");
        cmd_reg_.RegisterCommand<DELcmd>("DEL");
        cmd_reg_.RegisterCommand<EXISTScmd>("EXISTS");
        cmd_reg_.RegisterCommand<KEYScmd>("KEYS");
        cmd_reg_.RegisterCommand<FLUSHDBcmd>("FLUSHDB");
        cmd_reg_.RegisterCommand<CONFIGcmd>("CONFIG");
        cmd_reg_.RegisterCommand<DBSIZEcmd>("DBSIZE");
        cmd_reg_.RegisterCommand<MEMORYUSAGEcmd>("MEMORY");

        cmd_reg_.RegisterCommand<SADDset>("SADD");
        cmd_reg_.RegisterCommand<SREMset>("SREM");
        cmd_reg_.RegisterCommand<SISMEMBERset>("SISMEMBER");
        cmd_reg_.RegisterCommand<SMEMBERSset>("SMEMBERS");
        cmd_reg_.RegisterCommand<SCARDset>("SCARD");
        cmd_reg_.RegisterCommand<SUNIONset>("SUNION");
        cmd_reg_.RegisterCommand<SINTERset>("SINTER");
        cmd_reg_.RegisterCommand<SDIFFset>("SDIFF");
        cmd_reg_.RegisterCommand<SMOVEset>("SMOVE");

        cmd_reg_.RegisterCommand<GEOADDgeo>("GEOADD");
        cmd_reg_.RegisterCommand<GEOPOSgeo>("GEOPOS");
        cmd_reg_.RegisterCommand<GEODISTgeo>("GEODIST");
        cmd_reg_.RegisterCommand<GEOSEARCHgeo>("GEOSEARCH");
        cmd_reg_.RegisterCommand<GEOSEARCHSTOREgeo>("GEOSEARCHSTORE");

        db_.SetMaxMemory(db_size);
    }

    Engine(const Engine& other) = delete;
    Engine& operator=(const Engine& other) = delete;

    InterfaceCommand::ExecuteType Execute(const std::string& command);

    template <class NewCommand>
    void RegisterCommand(const std::string& name) {
        cmd_reg_.RegisterCommand<NewCommand>(name);
    }

    void Run();
private:
    DataBase db_;
    CommandRegister cmd_reg_;
};
