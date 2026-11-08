#include <gtest/gtest.h>
#include "ValueObject/Value.h"
#include "HelpingStructures.h"

class ValueTest : public ::testing::Test {
protected:
    void SetUp() override {
        AllocatorMemoryBlock::current_memory = 0;
        AllocatorMemoryBlock::max_memory = SIZE_MAX;
    }
    
    void TearDown() override {
        AllocatorMemoryBlock::current_memory = 0;
    }
};

TEST_F(ValueTest, NoneTypeMemoryUsage) {
    NoneType none;
    EXPECT_EQ(none.MemoryUsed(), 0);
}

TEST_F(ValueTest, StringTypeConstruction) {
    StringType str;
    EXPECT_EQ(str.Len(), 0);
    
    StringType str2("hello");
    EXPECT_EQ(str2.Len(), 5);
    EXPECT_GT(str2.MemoryUsed(), 0);
}

TEST_F(ValueTest, StringTypeAppend) {
    StringType str("hello");
    str.Append(" world");
    EXPECT_EQ(str.Len(), 11);
    EXPECT_EQ(std::string(str.data.c_str()), "hello world");
}

TEST_F(ValueTest, StringTypeReturnType) {
    StringType str("test");
    auto ret = str.ReturnTypeResult();
    EXPECT_EQ(ret.data, "test");
}

TEST_F(ValueTest, ListTypeOperations) {
    ListType list;
    EXPECT_TRUE(list.Empty());
    EXPECT_EQ(list.Len(), 0);
    
    list.PushBack("first");
    list.PushBack("second");
    EXPECT_FALSE(list.Empty());
    EXPECT_EQ(list.Len(), 2);
    
    list.PushFront("zero");
    EXPECT_EQ(list.Len(), 3);
    
    auto front = list.PopFront();
    EXPECT_EQ(front.data, "zero");
    
    auto back = list.PopBack();
    EXPECT_EQ(back.data, "second");
    EXPECT_EQ(list.Len(), 1);
}

TEST_F(ValueTest, ListTypePopEmpty) {
    ListType list;
    auto result = list.PopFront();
    EXPECT_TRUE(result.data.empty());
    
    result = list.PopBack();
    EXPECT_TRUE(result.data.empty());
}

TEST_F(ValueTest, ListTypeSetIndex) {
    ListType list;
    list.PushBack("a");
    list.PushBack("b");
    list.PushBack("c");
    
    list.SetIndex(1, "x");
    auto ret = list.ReturnTypeResult();
    EXPECT_EQ(ret.data[1], "x");
    
    list.SetIndex(10, "out");
    EXPECT_EQ(list.Len(), 3);
}

TEST_F(ValueTest, ListTypeMemoryUsage) {
    ListType list;
    size_t initial = list.MemoryUsed();
    
    list.PushBack("short");
    size_t after_one = list.MemoryUsed();
    EXPECT_GT(after_one, initial);
}

TEST_F(ValueTest, SetTypeOperations) {
    SetType set;
    EXPECT_EQ(set.Len(), 0);
    
    set.Add("one");
    set.Add("two");
    set.Add("one");
    EXPECT_EQ(set.Len(), 2);
    
    set.Remove("one");
    EXPECT_EQ(set.Len(), 1);
    
    set.Remove("nonexistent");
    EXPECT_EQ(set.Len(), 1);
}

TEST_F(ValueTest, SetTypeReturnType) {
    SetType set;
    set.Add("a");
    set.Add("b");
    set.Add("c");
    
    auto ret = set.ReturnTypeResult();
    EXPECT_EQ(ret.data.size(), 3);
}

TEST_F(ValueTest, GeoTypeAddPoint) {
    GeoType geo;
    geo.AddPoint("moscow", 55.7558, 37.6173);
    geo.AddPoint("spb", 59.9311, 30.3609);
    
    EXPECT_EQ(geo.data.size(), 2);
}

TEST_F(ValueTest, GeoTypeRemovePoint) {
    GeoType geo;
    geo.AddPoint("moscow", 55.7558, 37.6173);
    geo.RemovePoint("moscow");
    EXPECT_EQ(geo.data.size(), 0);
}

TEST_F(ValueTest, GeoTypeEverythingToKm) {
    EXPECT_DOUBLE_EQ(GeoType::EverythingToKm("km"), 1.0);
    EXPECT_DOUBLE_EQ(GeoType::EverythingToKm("m"), 1000.0);
    EXPECT_DOUBLE_EQ(GeoType::EverythingToKm("mi"), 0.621371);
    EXPECT_DOUBLE_EQ(GeoType::EverythingToKm("ft"), 3280.84);
    EXPECT_DOUBLE_EQ(GeoType::EverythingToKm("unknown"), 1000.0);
}

TEST_F(ValueTest, GeoTypeHaversine) {
    double dist = GeoType::Haversine(55.7558, 37.6173, 59.9311, 30.3609);
    EXPECT_GT(dist, 600);
    EXPECT_LT(dist, 700);
}

TEST_F(ValueTest, ValueContainerConstruction) {
    ValueContainer v1;
    EXPECT_TRUE(std::holds_alternative<NoneType>(v1.data_));
    EXPECT_EQ(v1.expires_, -1);
    
    ValueContainer v2(StringType("test"));
    EXPECT_TRUE(std::holds_alternative<StringType>(v2.data_));
}

TEST_F(ValueTest, ValueContainerMemoryUsed) {
    ValueContainer v;
    size_t initial = v.MemoryUsed();
    
    v = ValueContainer(StringType("hello world"));
    EXPECT_GT(v.MemoryUsed(), initial);
}

TEST_F(ValueTest, AllocatorMemoryTracking) {
    size_t before = AllocatorMemoryBlock::current_memory;
    
    DBAllocatorForTypes<int> alloc;
    int* ptr = alloc.allocate(10);
    
    size_t after = AllocatorMemoryBlock::current_memory;
    EXPECT_GT(after, before);
    
    alloc.deallocate(ptr, 10);
    EXPECT_EQ(AllocatorMemoryBlock::current_memory, before);
}

TEST_F(ValueTest, OverloadPattern) {
    auto visitor = OverloadPattern{
        [](int i) { return i * 2; },
        [](double d) { return d * 2.0; },
        [](const std::string& s) { return s + s; }
    };
    
    EXPECT_EQ(visitor(5), 10);
    EXPECT_DOUBLE_EQ(visitor(3.14), 6.28);
    EXPECT_EQ(visitor(std::string("hi")), "hihi");
}
