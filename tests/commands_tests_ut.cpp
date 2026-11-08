#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Engine/DataBase.h"
#include "BaseCommands/BaseCommands.h"
#include "BaseCommands/StringCommands.h"
#include "BaseCommands/ListCommands.h"
#include "BaseCommands/SetCommands.h"
#include "BaseCommands/GeoCommands.h"

class CommandsTest : public ::testing::Test {
protected:
    void SetUp() override {
        AllocatorMemoryBlock::current_memory = 0;
        AllocatorMemoryBlock::max_memory = SIZE_MAX;
    }
    
    void TearDown() override {
        db_.Flush();
        AllocatorMemoryBlock::current_memory = 0;
    }
    
    DataBase db_;
};


TEST_F(CommandsTest, String_SET_and_GET) {
    SETstring set_cmd;
    GETstring get_cmd;
    
    auto set_result = set_cmd.Execute(db_, {"SET", "key", "value"});
    EXPECT_TRUE(std::holds_alternative<StatusReturnType>(set_result));
    EXPECT_EQ(std::get<StatusReturnType>(set_result).status, "OK");
    
    auto get_result = get_cmd.Execute(db_, {"GET", "key"});
    EXPECT_TRUE(std::holds_alternative<StringReturnType>(get_result));
    EXPECT_EQ(std::get<StringReturnType>(get_result).data, "value");
}

TEST_F(CommandsTest, String_GET_Nonexistent) {
    GETstring get_cmd;
    auto result = get_cmd.Execute(db_, {"GET", "nonexistent"});
    EXPECT_TRUE(std::holds_alternative<NullType>(result));
}

TEST_F(CommandsTest, String_SET_WrongArgs) {
    SETstring set_cmd;
    auto result = set_cmd.Execute(db_, {"SET", "key"});
    EXPECT_TRUE(std::holds_alternative<ErrorReturnType>(result));
}

TEST_F(CommandsTest, String_STRLEN) {
    STRLENstring strlen_cmd;
    SETstring set_cmd;
    
    set_cmd.Execute(db_, {"SET", "key", "hello"});
    auto result = strlen_cmd.Execute(db_, {"STRLEN", "key"});
    EXPECT_TRUE(std::holds_alternative<IntegerReturnType>(result));
    EXPECT_EQ(std::get<IntegerReturnType>(result).data, 5);
}

TEST_F(CommandsTest, String_APPEND) {
    APPENDstring append_cmd;
    SETstring set_cmd;
    
    set_cmd.Execute(db_, {"SET", "key", "hello"});
    auto result = append_cmd.Execute(db_, {"APPEND", "key", " world"});
    EXPECT_TRUE(std::holds_alternative<IntegerReturnType>(result));
    EXPECT_EQ(std::get<IntegerReturnType>(result).data, 11);
    
    GETstring get_cmd;
    auto get_result = get_cmd.Execute(db_, {"GET", "key"});
    EXPECT_EQ(std::get<StringReturnType>(get_result).data, "hello world");
}

TEST_F(CommandsTest, String_EXPIRE_and_TTL) {
    EXPIREstring expire_cmd;
    TTLstring ttl_cmd;
    SETstring set_cmd;
    
    set_cmd.Execute(db_, {"SET", "key", "value"});
    
    auto expire_result = expire_cmd.Execute(db_, {"EXPIRE", "key", "100"});
    EXPECT_TRUE(std::holds_alternative<IntegerReturnType>(expire_result));
    EXPECT_EQ(std::get<IntegerReturnType>(expire_result).data, 1);
    
    auto ttl_result = ttl_cmd.Execute(db_, {"TTL", "key"});
    EXPECT_TRUE(std::holds_alternative<IntegerReturnType>(ttl_result));
    EXPECT_LE(std::get<IntegerReturnType>(ttl_result).data, 100);
}




TEST_F(CommandsTest, List_LPUSH_and_LRANGE) {
    LPUSHlist lpush_cmd;
    LRANGElist lrange_cmd;
    
    lpush_cmd.Execute(db_, {"LPUSH", "list", "a", "b", "c"});
    
    auto result = lrange_cmd.Execute(db_, {"LRANGE", "list", "0", "-1"});
    EXPECT_TRUE(std::holds_alternative<ArrayReturnType>(result));
    auto& arr = std::get<ArrayReturnType>(result).data;
    EXPECT_EQ(arr.size(), 3);
    EXPECT_EQ(arr[0], "c");
    EXPECT_EQ(arr[1], "b");
    EXPECT_EQ(arr[2], "a");
}

TEST_F(CommandsTest, List_LRANGE_Advanced) {
    RPUSHlist rpush;
    LRANGElist lrange;
    rpush.Execute(db_, {"RPUSH", "l", "1", "2", "3", "4", "5"});

    auto res1 = lrange.Execute(db_, {"LRANGE", "l", "0", "-1"});
    EXPECT_EQ(std::get<ArrayReturnType>(res1).data.size(), 5);

    auto res2 = lrange.Execute(db_, {"LRANGE", "l", "-2", "-1"});
    auto& data2 = std::get<ArrayReturnType>(res2).data;
    ASSERT_EQ(data2.size(), 2);
    EXPECT_EQ(data2[0], "4");
    EXPECT_EQ(data2[1], "5");

    auto res3 = lrange.Execute(db_, {"LRANGE", "l", "10", "20"});
    EXPECT_EQ(std::get<ArrayReturnType>(res3).data.size(), 0);

    auto res4 = lrange.Execute(db_, {"LRANGE", "l", "-100", "1"});
    EXPECT_EQ(std::get<ArrayReturnType>(res4).data.size(), 2);
}

TEST_F(CommandsTest, List_LRANGE_EdgeCases) {
    RPUSHlist rpush;
    LRANGElist lrange;
    rpush.Execute(db_, {"RPUSH", "l", "1", "2", "3"});

    auto res1 = lrange.Execute(db_, {"LRANGE", "l", "2", "1"});
    EXPECT_EQ(std::get<ArrayReturnType>(res1).data.size(), 0);

    auto res2 = lrange.Execute(db_, {"LRANGE", "l", "0", "100"});
    EXPECT_EQ(std::get<ArrayReturnType>(res2).data.size(), 3);

    auto res3 = lrange.Execute(db_, {"LRANGE", "l", "-100", "-50"});
    EXPECT_EQ(std::get<ArrayReturnType>(res3).data.size(), 0);
}

TEST_F(CommandsTest, List_RPUSH_and_LPOP) {
    RPUSHlist rpush_cmd;
    LPOPlist lpop_cmd;
    LLENlist llen_cmd;
    
    rpush_cmd.Execute(db_, {"RPUSH", "list", "a", "b"});
    
    auto result = lpop_cmd.Execute(db_, {"LPOP", "list"});
    EXPECT_TRUE(std::holds_alternative<StringReturnType>(result));
    EXPECT_EQ(std::get<StringReturnType>(result).data, "a");

    auto resukt_len = llen_cmd.Execute(db_, {"LLEN", "list"});
    EXPECT_TRUE(std::holds_alternative<IntegerReturnType>(resukt_len));
    EXPECT_EQ(std::get<IntegerReturnType>(resukt_len).data, 1);
}

TEST_F(CommandsTest, List_RPOP) {
    RPUSHlist rpush_cmd;
    RPOPlist rpop_cmd;
    
    rpush_cmd.Execute(db_, {"RPUSH", "list", "a", "b", "c"});
    
    auto result = rpop_cmd.Execute(db_, {"RPOP", "list"});
    EXPECT_TRUE(std::holds_alternative<StringReturnType>(result));
    EXPECT_EQ(std::get<StringReturnType>(result).data, "c");

    auto r1 = rpop_cmd.Execute(db_, {"RPOP", "list"});
    EXPECT_TRUE(std::holds_alternative<StringReturnType>(r1));
    EXPECT_EQ(std::get<StringReturnType>(r1).data, "b");

    auto r2 = rpop_cmd.Execute(db_, {"RPOP", "list"});
    EXPECT_TRUE(std::holds_alternative<StringReturnType>(r2));
    EXPECT_EQ(std::get<StringReturnType>(r2).data, "a");

    auto r3 = rpop_cmd.Execute(db_, {"RPOP", "list"});
    EXPECT_TRUE(std::holds_alternative<NullType>(r3));
}

TEST_F(CommandsTest, List_LLEN) {
    LLENlist llen_cmd;
    LPUSHlist lpush_cmd;
    
    lpush_cmd.Execute(db_, {"LPUSH", "list", "a", "b"});
    
    auto result = llen_cmd.Execute(db_, {"LLEN", "list"});
    EXPECT_TRUE(std::holds_alternative<IntegerReturnType>(result));
    EXPECT_EQ(std::get<IntegerReturnType>(result).data, 2);
}

TEST_F(CommandsTest, List_LINDEX) {
    LINDEXlist lindex_cmd;
    RPUSHlist rpush_cmd;
    
    rpush_cmd.Execute(db_, {"RPUSH", "list", "a", "b", "c"});
    
    auto result = lindex_cmd.Execute(db_, {"LINDEX", "list", "1"});
    EXPECT_TRUE(std::holds_alternative<StringReturnType>(result));
    EXPECT_EQ(std::get<StringReturnType>(result).data, "b");
}

TEST_F(CommandsTest, List_LSET) {
    LSETlist lset_cmd;
    RPUSHlist rpush_cmd;
    LINDEXlist lindex_cmd;
    
    rpush_cmd.Execute(db_, {"RPUSH", "list", "a", "b", "c"});
    lset_cmd.Execute(db_, {"LSET", "list", "1", "x"});
    
    auto result = lindex_cmd.Execute(db_, {"LINDEX", "list", "1"});
    EXPECT_EQ(std::get<StringReturnType>(result).data, "x");
}

TEST_F(CommandsTest, List_LINSERT) {
    LINSERTlist linsert_cmd;
    RPUSHlist rpush_cmd;
    LRANGElist lrange_cmd;
    
    rpush_cmd.Execute(db_, {"RPUSH", "list", "a", "b", "c"});
    linsert_cmd.Execute(db_, {"LINSERT", "list", "BEFORE", "b", "x"});
    
    auto result = lrange_cmd.Execute(db_, {"LRANGE", "list", "0", "-1"});
    auto& arr = std::get<ArrayReturnType>(result).data;
    EXPECT_EQ(arr.size(), 4);
    EXPECT_EQ(arr[1], "x");
    EXPECT_EQ(arr[2], "b");
}




TEST_F(CommandsTest, Set_SADD_and_SMEMBERS) {
    SADDset sadd_cmd;
    SMEMBERSset smembers_cmd;
    
    auto result = sadd_cmd.Execute(db_, {"SADD", "set", "a", "b", "c"});
    EXPECT_TRUE(std::holds_alternative<IntegerReturnType>(result));
    EXPECT_EQ(std::get<IntegerReturnType>(result).data, 3);
    
    auto members = smembers_cmd.Execute(db_, {"SMEMBERS", "set"});
    EXPECT_TRUE(std::holds_alternative<ArrayReturnType>(members));
    EXPECT_EQ(std::get<ArrayReturnType>(members).data.size(), 3);
}

TEST_F(CommandsTest, List_LSET_Errors) {
    LSETlist lset;
    RPUSHlist rpush;

    auto res1 = lset.Execute(db_, {"LSET", "no_list", "0", "val"});
    EXPECT_TRUE(std::holds_alternative<ErrorReturnType>(res1));

    rpush.Execute(db_, {"RPUSH", "mylist", "a"});
    auto res2 = lset.Execute(db_, {"LSET", "mylist", "5", "val"});
    EXPECT_TRUE(std::holds_alternative<ErrorReturnType>(res2));
}

TEST_F(CommandsTest, Set_Complex_SINTER) {
    SADDset sadd;
    SINTERset sinter;
    SETstring set_str;

    sadd.Execute(db_, {"SADD", "set1", "a", "b"});

    auto res1 = sinter.Execute(db_, {"SINTER", "set1", "nonexistent"});
    EXPECT_EQ(std::get<ArrayReturnType>(res1).data.size(), 0);

    set_str.Execute(db_, {"SET", "not_a_set", "value"});
    auto res2 = sinter.Execute(db_, {"SINTER", "set1", "not_a_set"});
    EXPECT_TRUE(std::holds_alternative<ErrorReturnType>(res2));
}

TEST_F(CommandsTest, Set_SREM) {
    SREMset srem_cmd;
    SADDset sadd_cmd;
    SCARDset scard_cmd;
    
    sadd_cmd.Execute(db_, {"SADD", "set", "a", "b", "c"});
    srem_cmd.Execute(db_, {"SREM", "set", "a", "c", "d"});
    
    auto result = scard_cmd.Execute(db_, {"SCARD", "set"});
    EXPECT_EQ(std::get<IntegerReturnType>(result).data, 1);
}

TEST_F(CommandsTest, Set_SISMEMBER) {
    SISMEMBERset sismember_cmd;
    SADDset sadd_cmd;
    
    sadd_cmd.Execute(db_, {"SADD", "set", "a"});
    
    auto result1 = sismember_cmd.Execute(db_, {"SISMEMBER", "set", "a"});
    EXPECT_EQ(std::get<IntegerReturnType>(result1).data, 1);
    
    auto result2 = sismember_cmd.Execute(db_, {"SISMEMBER", "set", "b"});
    EXPECT_EQ(std::get<IntegerReturnType>(result2).data, 0);

    sadd_cmd.Execute(db_, {"SADD", "set", "b"});
    result2 = sismember_cmd.Execute(db_, {"SISMEMBER", "set", "b"});
    EXPECT_EQ(std::get<IntegerReturnType>(result2).data, 1);
}

TEST_F(CommandsTest, Set_SUNION) {
    SUNIONset sunion_cmd;
    SADDset sadd_cmd;
    
    sadd_cmd.Execute(db_, {"SADD", "set1", "a", "b"});
    sadd_cmd.Execute(db_, {"SADD", "set2", "b", "c"});
    
    auto result = sunion_cmd.Execute(db_, {"SUNION", "set1", "set2"});
    EXPECT_TRUE(std::holds_alternative<ArrayReturnType>(result));
    EXPECT_EQ(std::get<ArrayReturnType>(result).data.size(), 3);
}

TEST_F(CommandsTest, Set_SINTER) {
    SINTERset sinter_cmd;
    SADDset sadd_cmd;
    
    sadd_cmd.Execute(db_, {"SADD", "set1", "a", "b", "c"});
    sadd_cmd.Execute(db_, {"SADD", "set2", "b", "c", "d"});
    
    auto result = sinter_cmd.Execute(db_, {"SINTER", "set1", "set2"});
    EXPECT_TRUE(std::holds_alternative<ArrayReturnType>(result));
    EXPECT_EQ(std::get<ArrayReturnType>(result).data.size(), 2);
}

TEST_F(CommandsTest, Set_SDIFF) {
    SDIFFset sdiff_cmd;
    SADDset sadd_cmd;
    
    sadd_cmd.Execute(db_, {"SADD", "set1", "a", "b", "c"});
    sadd_cmd.Execute(db_, {"SADD", "set2", "b"});
    
    auto result = sdiff_cmd.Execute(db_, {"SDIFF", "set1", "set2"});
    EXPECT_TRUE(std::holds_alternative<ArrayReturnType>(result));
    EXPECT_EQ(std::get<ArrayReturnType>(result).data.size(), 2);
}

TEST_F(CommandsTest, Set_SMOVE) {
    SMOVEset smove_cmd;
    SADDset sadd_cmd;
    SISMEMBERset sismember_cmd;
    
    sadd_cmd.Execute(db_, {"SADD", "set1", "a"});
    
    auto result = smove_cmd.Execute(db_, {"SMOVE", "set1", "set2", "a"});
    EXPECT_EQ(std::get<IntegerReturnType>(result).data, 1);
    
    auto check1 = sismember_cmd.Execute(db_, {"SISMEMBER", "set1", "a"});
    EXPECT_EQ(std::get<IntegerReturnType>(check1).data, 0);
    
    auto check2 = sismember_cmd.Execute(db_, {"SISMEMBER", "set2", "a"});
    EXPECT_EQ(std::get<IntegerReturnType>(check2).data, 1);
}




TEST_F(CommandsTest, Geo_GEOADD) {
    GEOADDgeo geoadd_cmd;
    
    auto result = geoadd_cmd.Execute(db_, {"GEOADD", "cities", "13.361389", "38.115556", "Palermo"});
    EXPECT_TRUE(std::holds_alternative<IntegerReturnType>(result));
    EXPECT_EQ(std::get<IntegerReturnType>(result).data, 1);
}

TEST_F(CommandsTest, Geo_GEOSEARCH_Full) {
    GEOADDgeo add;
    GEOSEARCHgeo search;
    GEOSEARCHSTOREgeo store;

    add.Execute(db_, {"GEOADD", "sicily", "13.361389", "38.115556", "Palermo"});
    add.Execute(db_, {"GEOADD", "sicily", "15.087269", "37.502669", "Catania"});
    add.Execute(db_, {"GEOADD", "sicily", "13.576367", "37.310567", "Agrigento"});

    auto res = search.Execute(db_, {"GEOSEARCH", "sicily", "FROMLONLAT", "13.361389", "38.115556", "BYRADIUS", "100", "km", "ASC"});
    auto& names = std::get<ArrayReturnType>(res).data;
    ASSERT_EQ(names.size(), 2);
    EXPECT_EQ(names[0], "Palermo\ndistance: 0.000000\nlon: 13.361389\nlat: 38.115556");
    EXPECT_EQ(names[1], "Agrigento\ndistance: 91.512196\nlon: 13.576367\nlat: 37.310567");

    store.Execute(db_, {"GEOSEARCHSTORE", "near_me", "sicily", "FROMLONLAT", "13.361389", "38.115556", "BYRADIUS", "100", "km"});
    
    LLENlist llen;
    auto len_res = llen.Execute(db_, {"LLEN", "near_me"});
    EXPECT_EQ(std::get<IntegerReturnType>(len_res).data, 2);
}

TEST_F(CommandsTest, Geo_GEOPOS) {
    GEOPOSgeo geopos_cmd;
    GEOADDgeo geoadd_cmd;
    
    geoadd_cmd.Execute(db_, {"GEOADD", "cities", "13.361389", "38.115556", "Palermo"});
    
    auto result = geopos_cmd.Execute(db_, {"GEOPOS", "cities", "Palermo"});
    EXPECT_TRUE(std::holds_alternative<GeoArrayReturnType>(result));
}

TEST_F(CommandsTest, Geo_GEODIST) {
    GEODISTgeo geodist_cmd;
    GEOADDgeo geoadd_cmd;
    
    geoadd_cmd.Execute(db_, {"GEOADD", "cities", "13.361389", "38.115556", "Palermo"});
    geoadd_cmd.Execute(db_, {"GEOADD", "cities", "15.087269", "37.502669", "Catania"});
    
    auto result = geodist_cmd.Execute(db_, {"GEODIST", "cities", "Palermo", "Catania"});
    EXPECT_TRUE(std::holds_alternative<StringReturnType>(result));
}




TEST_F(CommandsTest, Base_TYPE) {
    TYPEcmd type_cmd;
    SETstring set_cmd;
    
    set_cmd.Execute(db_, {"SET", "str", "value"});
    auto result = type_cmd.Execute(db_, {"TYPE", "str"});
    EXPECT_EQ(std::get<StatusReturnType>(result).status, "String");
}

TEST_F(CommandsTest, Base_KEYS_Advanced) {
    SETstring set;
    KEYScmd keys;
    set.Execute(db_, {"SET", "hallo", "1"});
    set.Execute(db_, {"SET", "hello", "1"});
    set.Execute(db_, {"SET", "hxllo", "1"});

    auto res1 = keys.Execute(db_, {"KEYS", "h?llo"});
    EXPECT_EQ(std::get<ArrayReturnType>(res1).data.size(), 3);

    auto res2 = keys.Execute(db_, {"KEYS", "h[ae]llo"});
    EXPECT_EQ(std::get<ArrayReturnType>(res2).data.size(), 2);

    auto res3 = keys.Execute(db_, {"KEYS", "*e[l]*"});
    EXPECT_EQ(std::get<ArrayReturnType>(res3).data[0], "hello");
}

TEST_F(CommandsTest, Base_DEL) {
    DELcmd del_cmd;
    SETstring set_cmd;
    
    set_cmd.Execute(db_, {"SET", "k1", "v1"});
    set_cmd.Execute(db_, {"SET", "k2", "v2"});
    
    auto result = del_cmd.Execute(db_, {"DEL", "k1", "k2"});
    EXPECT_EQ(std::get<IntegerReturnType>(result).data, 2);
}

TEST_F(CommandsTest, Base_EXISTS) {
    EXISTScmd exists_cmd;
    SETstring set_cmd;
    
    set_cmd.Execute(db_, {"SET", "k1", "v1"});
    
    auto result = exists_cmd.Execute(db_, {"EXISTS", "k1", "k2"});
    EXPECT_EQ(std::get<IntegerReturnType>(result).data, 1);
}

TEST_F(CommandsTest, Base_KEYS) {
    KEYScmd keys_cmd;
    SETstring set_cmd;
    
    set_cmd.Execute(db_, {"SET", "foo", "v"});
    set_cmd.Execute(db_, {"SET", "foobar", "v"});
    set_cmd.Execute(db_, {"SET", "bar", "v"});
    
    auto result = keys_cmd.Execute(db_, {"KEYS", "foo*"});
    EXPECT_TRUE(std::holds_alternative<ArrayReturnType>(result));
    EXPECT_EQ(std::get<ArrayReturnType>(result).data.size(), 2);
}

TEST_F(CommandsTest, Base_FLUSHDB) {
    FLUSHDBcmd flush_cmd;
    SETstring set_cmd;
    
    set_cmd.Execute(db_, {"SET", "key", "value"});
    flush_cmd.Execute(db_, {"FLUSHDB"});
    
    EXPECT_FALSE(db_.IsKeyExist("key"));
}

TEST_F(CommandsTest, Base_DBSIZE) {
    DBSIZEcmd dbsize_cmd;
    SETstring set_cmd;
    
    set_cmd.Execute(db_, {"SET", "k1", "v1"});
    set_cmd.Execute(db_, {"SET", "k2", "v2"});
    
    auto result = dbsize_cmd.Execute(db_, {"DBSIZE"});
    EXPECT_EQ(std::get<IntegerReturnType>(result).data, 2);
}

TEST_F(CommandsTest, Base_CONFIG) {
    CONFIGcmd config_cmd;
    
    auto get_result = config_cmd.Execute(db_, {"CONFIG", "GET", "maxmemory"});
    EXPECT_TRUE(std::holds_alternative<ArrayReturnType>(get_result));
    EXPECT_EQ(SIZE_MAX, AllocatorMemoryBlock::max_memory);
    
    auto set_result = config_cmd.Execute(db_, {"CONFIG", "SET", "maxmemory", "1000000"});
    EXPECT_TRUE(std::holds_alternative<StatusReturnType>(set_result));
    EXPECT_EQ(1000000, AllocatorMemoryBlock::max_memory);
}

TEST_F(CommandsTest, Base_MEMORYUSAGE) {
    MEMORYUSAGEcmd memory_cmd;
    SETstring set_cmd;
    
    set_cmd.Execute(db_, {"SET", "key", "value"});
    
    auto result = memory_cmd.Execute(db_, {"MEMORY", "USAGE", "key"});
    EXPECT_TRUE(std::holds_alternative<IntegerReturnType>(result));
    EXPECT_GT(std::get<IntegerReturnType>(result).data, 0);
}

TEST_F(CommandsTest, Global_TypeSafety) {
    SETstring set;
    LPUSHlist lpush;
    SADDset sadd;

    set.Execute(db_, {"SET", "strict", "value"});

    auto res1 = lpush.Execute(db_, {"LPUSH", "strict", "item"});
    EXPECT_TRUE(std::holds_alternative<ErrorReturnType>(res1));
    EXPECT_THAT(std::get<ErrorReturnType>(res1).error_message, ::testing::HasSubstr("wrongtype"));

    auto res2 = sadd.Execute(db_, {"SADD", "strict", "member"});
    EXPECT_TRUE(std::holds_alternative<ErrorReturnType>(res2));
}

TEST_F(CommandsTest, TTL_Reset_On_Overwrite) {
    SETstring set_cmd;
    EXPIREstring expire_cmd;
    TTLstring ttl_cmd;

    set_cmd.Execute(db_, {"SET", "key", "val1"});
    expire_cmd.Execute(db_, {"EXPIRE", "key", "100"});

    auto ttl1 = ttl_cmd.Execute(db_, {"TTL", "key"});
    EXPECT_GT(std::get<IntegerReturnType>(ttl1).data, 0);

    set_cmd.Execute(db_, {"SET", "key", "val2"});

    auto ttl2 = ttl_cmd.Execute(db_, {"TTL", "key"});
    EXPECT_EQ(std::get<IntegerReturnType>(ttl2).data, -1);
}
