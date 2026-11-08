#include <gtest/gtest.h>
#include "Engine/DataBase.h"
#include "ValueObject/Value.h"

class MemoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        AllocatorMemoryBlock::current_memory = 0;
    }
    
    void TearDown() override {
        AllocatorMemoryBlock::current_memory = 0;
        AllocatorMemoryBlock::max_memory = SIZE_MAX;
    }
};

TEST_F(MemoryTest, StringMemoryUsage) {
    StringType str;
    size_t base = str.MemoryUsed();
    
    StringType str2("hello3242342342343243224");
    size_t with_data = str2.MemoryUsed();
    EXPECT_GT(with_data, base);
}

TEST_F(MemoryTest, ListMemoryUsage) {
    ListType list;
    size_t base = list.MemoryUsed();
    
    list.PushBack("item1");
    size_t after_one = list.MemoryUsed();
    EXPECT_GT(after_one, base);
    
    list.PushBack("item2");
    size_t after_two = list.MemoryUsed();
    EXPECT_GT(after_two, after_one);
}

TEST_F(MemoryTest, SetMemoryUsage) {
    SetType set;
    size_t base = set.MemoryUsed();
    
    set.Add("element");
    size_t after = set.MemoryUsed();
    EXPECT_GT(after, base);
}

TEST_F(MemoryTest, GeoMemoryUsage) {
    GeoType geo;
    size_t base = geo.MemoryUsed();
    
    geo.AddPoint("point1", 55.7558, 37.6173);
    size_t after = geo.MemoryUsed();
    EXPECT_GT(after, base);
}

TEST_F(MemoryTest, ValueContainerMemory) {
    ValueContainer v;
    size_t base = v.MemoryUsed();
    
    v = ValueContainer(StringType("some data here"));
    size_t with_string = v.MemoryUsed();
    EXPECT_GT(with_string, base);
}

TEST_F(MemoryTest, MaxMemoryLimit) {
    AllocatorMemoryBlock::max_memory = 1000;
    
    DBAllocatorForTypes<char> alloc;
    EXPECT_THROW(alloc.allocate(2000), std::bad_alloc);
    
    char* ptr = alloc.allocate(500);
    ASSERT_NE(ptr, nullptr);
    
    alloc.deallocate(ptr, 500);
}

TEST_F(MemoryTest, CurrentMemoryTracking) {
    size_t initial = AllocatorMemoryBlock::current_memory;
    
    DBAllocatorForTypes<int> alloc;
    int* p1 = alloc.allocate(10);
    size_t after_alloc1 = AllocatorMemoryBlock::current_memory;
    EXPECT_GT(after_alloc1, initial);
    
    int* p2 = alloc.allocate(20);
    size_t after_alloc2 = AllocatorMemoryBlock::current_memory;
    EXPECT_GT(after_alloc2, after_alloc1);
    
    alloc.deallocate(p1, 10);
    size_t after_dealloc1 = AllocatorMemoryBlock::current_memory;
    EXPECT_LT(after_dealloc1, after_alloc2);
    
    alloc.deallocate(p2, 20);
    EXPECT_EQ(AllocatorMemoryBlock::current_memory, initial);
}

TEST_F(MemoryTest, Allocator_Stress_LeakCheck) {
    size_t initial_mem = AllocatorMemoryBlock::current_memory;
    
    {
        DataBase temp_db;
        for(int i = 0; i < 1000; ++i) {
            temp_db.Set("key" + std::to_string(i), StringReturnType("value" + std::to_string(i)));
        }
        EXPECT_GT(AllocatorMemoryBlock::current_memory, initial_mem);
        temp_db.Flush();
    }

    EXPECT_EQ(AllocatorMemoryBlock::current_memory, initial_mem);
}
