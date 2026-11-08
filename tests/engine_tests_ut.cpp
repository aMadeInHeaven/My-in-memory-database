#include <gtest/gtest.h>
#include "Engine/Engine.h"

class EngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        AllocatorMemoryBlock::current_memory = 0;
        AllocatorMemoryBlock::max_memory = SIZE_MAX;
    }
    
    void TearDown() override {
        AllocatorMemoryBlock::current_memory = 0;
    }
};

TEST_F(EngineTest, ExecuteSetAndGet) {
    Engine engine;
    
    auto set_result = engine.Execute("SET mykey myvalue");
    EXPECT_TRUE(std::holds_alternative<StatusReturnType>(set_result));
    EXPECT_EQ(std::get<StatusReturnType>(set_result).status, "OK");
    
    auto get_result = engine.Execute("GET mykey");
    EXPECT_TRUE(std::holds_alternative<StringReturnType>(get_result));
    EXPECT_EQ(std::get<StringReturnType>(get_result).data, "myvalue");
}

TEST_F(EngineTest, ExecuteUnknownCommand) {
    Engine engine;
    
    auto result = engine.Execute("UNKNOWN command");
    EXPECT_TRUE(std::holds_alternative<ErrorReturnType>(result));
    std::string error = std::get<ErrorReturnType>(result).error_message;
    EXPECT_TRUE(error.find("unknown command") != std::string::npos);
}

TEST_F(EngineTest, ExecuteCaseInsensitiveCommand) {
    Engine engine;
    
    auto result1 = engine.Execute("set key value");
    auto result2 = engine.Execute("GET key");
    
    EXPECT_TRUE(std::holds_alternative<StringReturnType>(result2));
    EXPECT_EQ(std::get<StringReturnType>(result2).data, "value");
}

TEST_F(EngineTest, ExecuteListCommands) {
    Engine engine;
    
    engine.Execute("LPUSH list item1 item2");
    engine.Execute("RPUSH list item3");
    
    auto result = engine.Execute("LRANGE list 0 -1");
    EXPECT_TRUE(std::holds_alternative<ArrayReturnType>(result));
    auto& arr = std::get<ArrayReturnType>(result).data;
    EXPECT_GE(arr.size(), 3);
}

TEST_F(EngineTest, ExecuteSetCommands) {
    Engine engine;
    
    engine.Execute("SADD set a b c");
    auto result = engine.Execute("SMEMBERS set");
    EXPECT_TRUE(std::holds_alternative<ArrayReturnType>(result));
    EXPECT_EQ(std::get<ArrayReturnType>(result).data.size(), 3);
}

TEST_F(EngineTest, ExecuteGeoCommands) {
    Engine engine;
    
    engine.Execute("GEOADD cities 13.361389 38.115556 Palermo");
    engine.Execute("GEOADD cities 15.087269 37.502669 Catania");
    
    auto result = engine.Execute("GEODIST cities Palermo Catania");
    EXPECT_TRUE(std::holds_alternative<StringReturnType>(result));
}

TEST_F(EngineTest, ExecuteWithInvalidArgs) {
    Engine engine;
    
    auto result = engine.Execute("SET key");
    EXPECT_TRUE(std::holds_alternative<ErrorReturnType>(result));
}

TEST_F(EngineTest, RegisterCustomCommand) {
    Engine engine;

    engine.RegisterCommand<SETstring>("CUSTOM");
}

TEST_F(EngineTest, MemoryLimitRespected) {
    AllocatorMemoryBlock::max_memory = 100;
    Engine engine(100);
    
    auto result = engine.Execute("SET key " + std::string(1000, 'x'));
    EXPECT_TRUE(std::holds_alternative<ErrorReturnType>(result));
}

TEST_F(EngineTest, ExecuteNestedCommands) {
    Engine engine;
    
    engine.Execute("SET key1 value1");
    engine.Execute("SET key2 value2");
    
    auto result = engine.Execute("DEL key1 key2");
    EXPECT_TRUE(std::holds_alternative<IntegerReturnType>(result));
    EXPECT_EQ(std::get<IntegerReturnType>(result).data, 2);
}
