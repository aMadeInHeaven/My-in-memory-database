#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>
#include <optional>
#include <chrono>
#include <iostream>
#include <random>

#include "../ValueObject/Value.h"
#include "../ReturnTypes/ReturnTypes.h"

static inline const int64_t kDBtimeDelta = 5;

class DataBase {
public:
    DataBase() : generator_(std::chrono::system_clock::now().time_since_epoch().count()) {}

    static int64_t Now() {
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    ReturnTypesVariant Get(const std::string& key);

    ValueContainer* GetPtr(const std::string& key);

    void Set(const std::string& key, const ReturnTypesVariant& data);

    void SetTTL(const std::string& key, int64_t ttl);

    void Delete(const std::string& key) {
        data_.erase(key);
        ttl_.erase(key);
    }

    bool IsKeyExist(const std::string& key);

    bool IsKeyExpired(const std::string& key);

    void SetMaxMemory(size_t memory) {
        if (AllocatorMemoryBlock::current_memory > memory) {
            std::cerr << "Can't update max_memory, because current_memory > your new one";
        } else {
            AllocatorMemoryBlock::max_memory = memory;
        }
    }

    std::vector<std::string> GetAllKeys() {
        DeleteExpiredKeys(true);

        std::vector<std::string> result;
        result.reserve(data_.size());

        for (const auto& [k, v] : data_) {
            result.push_back(k);
        }

        return result;
    }

    size_t GetKeysAmmount() {
        DeleteExpiredKeys(true);
        return data_.size();
    }

    size_t GetMaxMemory() const {
        return AllocatorMemoryBlock::max_memory;
    }

    size_t GetUsedMemory() {
        DeleteExpiredKeys(true);
        return AllocatorMemoryBlock::current_memory;
    }

    int64_t TimeLeftUntilExpired(const std::string& key);

    void DeleteExpiredKeys(bool force = false);

    void Flush() {
        data_.clear();
        ttl_.clear();
    }

    std::string GetTypeName(const std::string& key) {
        DeleteExpiredKeys();

        if (IsKeyExpired(key)) {
            Delete(key);
            return "None";
        }

        auto it = data_.find(key);

        if (it == data_.end()) {
            return "None";
        }

        return std::visit(OverloadPattern {
            [](const StringType&) { return "String"; },
            [](const ListType&) { return "List"; },
            [](const SetType&) { return "Set"; },
            [](const GeoType&) { return "Geo"; },
            [](const NoneType&) { return "None"; }
        }, it->second.data_);
    }
private:
    std::unordered_map<
        std::string, ValueContainer, std::hash<std::string>, 
        std::equal_to<std::string>, Allocator<std::pair<const std::string, ValueContainer>>
    > data_;

    std::unordered_map<
        std::string, int64_t, std::hash<std::string>, 
        std::equal_to<std::string>, Allocator<std::pair<const std::string, int64_t>>
    > ttl_;

    std::mt19937 generator_;
};
