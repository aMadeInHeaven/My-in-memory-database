#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Engine/DataBase.h"

class DataBaseTest : public ::testing::Test {
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

TEST_F(DataBaseTest, SetAndGetString) {
    db_.Set("key1", StringReturnType("value1"));
    
    auto result = db_.Get("key1");
    EXPECT_TRUE(std::holds_alternative<StringReturnType>(result));
    EXPECT_EQ(std::get<StringReturnType>(result).data, "value1");
}

TEST_F(DataBaseTest, SetAndGetList) {
    std::vector<std::string> data = {"a", "b", "c"};
    db_.Set("mylist", ArrayReturnType(data));
    
    auto result = db_.Get("mylist");
    EXPECT_TRUE(std::holds_alternative<ArrayReturnType>(result));
    auto& arr = std::get<ArrayReturnType>(result).data;
    EXPECT_EQ(arr.size(), 3);
    EXPECT_THAT(arr, ::testing::ElementsAre("a", "b", "c"));
}

TEST_F(DataBaseTest, GetNonexistentKey) {
    auto result = db_.Get("nonexistent");
    EXPECT_TRUE(std::holds_alternative<NullType>(result));
}

TEST_F(DataBaseTest, GetPtr) {
    db_.Set("key", StringReturnType("value"));
    auto* ptr = db_.GetPtr("key");
    ASSERT_NE(ptr, nullptr);
    EXPECT_TRUE(std::holds_alternative<StringType>(ptr->data_));
    
    auto* ptr2 = db_.GetPtr("nonexistent");
    EXPECT_EQ(ptr2, nullptr);
}

TEST_F(DataBaseTest, DeleteKey) {
    db_.Set("key", StringReturnType("value"));
    EXPECT_TRUE(db_.IsKeyExist("key"));
    
    db_.Delete("key");
    EXPECT_FALSE(db_.IsKeyExist("key"));
}

TEST_F(DataBaseTest, IsKeyExist) {
    EXPECT_FALSE(db_.IsKeyExist("key"));
    
    db_.Set("key", StringReturnType("value"));
    EXPECT_TRUE(db_.IsKeyExist("key"));
}

TEST_F(DataBaseTest, TTLBasic) {
    db_.Set("key", StringReturnType("value"));
    db_.SetTTL("key", 10);
    
    int64_t ttl = db_.TimeLeftUntilExpired("key");
    EXPECT_LE(ttl, 10);
    EXPECT_GE(ttl, 0);
}

TEST_F(DataBaseTest, TTLNoExpire) {
    db_.Set("key", StringReturnType("value"));
    EXPECT_EQ(db_.TimeLeftUntilExpired("key"), -1);
}

TEST_F(DataBaseTest, GetAllKeys) {
    db_.Set("key1", StringReturnType("v1"));
    db_.Set("key2", StringReturnType("v2"));
    db_.Set("key3", StringReturnType("v3"));
    
    auto keys = db_.GetAllKeys();
    EXPECT_EQ(keys.size(), 3);
    EXPECT_THAT(keys, ::testing::UnorderedElementsAre("key1", "key2", "key3"));
}

TEST_F(DataBaseTest, GetKeysAmount) {
    EXPECT_EQ(db_.GetKeysAmmount(), 0);
    
    db_.Set("key1", StringReturnType("v1"));
    EXPECT_EQ(db_.GetKeysAmmount(), 1);
    
    db_.Set("key2", StringReturnType("v2"));
    EXPECT_EQ(db_.GetKeysAmmount(), 2);
}

TEST_F(DataBaseTest, GetTypeName) {
    db_.Set("str_key", StringReturnType("value"));
    EXPECT_EQ(db_.GetTypeName("str_key"), "String");
    
    db_.Set("list_key", ArrayReturnType());
    EXPECT_EQ(db_.GetTypeName("list_key"), "List");
    
    EXPECT_EQ(db_.GetTypeName("nonexistent"), "None");
}

TEST_F(DataBaseTest, Flush) {
    db_.Set("key1", StringReturnType("v1"));
    db_.Set("key2", StringReturnType("v2"));
    EXPECT_EQ(db_.GetKeysAmmount(), 2);
    
    db_.Flush();
    EXPECT_EQ(db_.GetKeysAmmount(), 0);
}

TEST_F(DataBaseTest, OverwriteKey) {
    db_.Set("key", StringReturnType("first"));
    auto result1 = db_.Get("key");
    EXPECT_EQ(std::get<StringReturnType>(result1).data, "first");
    
    db_.Set("key", StringReturnType("second"));
    auto result2 = db_.Get("key");
    EXPECT_EQ(std::get<StringReturnType>(result2).data, "second");
}

TEST_F(DataBaseTest, NowFunction) {
    int64_t now = DataBase::Now();
    EXPECT_GT(now, 0);
    
    int64_t now2 = DataBase::Now();
    EXPECT_GE(now2, now);
}
