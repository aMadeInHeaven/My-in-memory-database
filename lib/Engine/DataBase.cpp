#include "DataBase.h"

ReturnTypesVariant DataBase::Get(const std::string& key) {
    DeleteExpiredKeys();

    if (IsKeyExpired(key)) {
        Delete(key);
        return NullType();
    }

    auto it = data_.find(key);

    if (it == data_.end()) {
        return NullType();
    }

    return std::visit([](auto&& arg) -> ReturnTypesVariant {
        return arg.ReturnTypeResult();
    }, it->second.data_);
}

ValueContainer* DataBase::GetPtr(const std::string& key) {
    DeleteExpiredKeys();

    if (IsKeyExpired(key)) {
        Delete(key);
        return nullptr;
    }

    auto it = data_.find(key);

    if (it == data_.end()) {
        return nullptr;
    }

    return &(it->second);
}

void DataBase::Set(const std::string& key, const ReturnTypesVariant& data) {
    DeleteExpiredKeys(true);

    ttl_.erase(key); 

    ValueContainer container = std::visit([](auto&& arg) -> ValueContainer {
        return ValueContainer::ConstructFromReturnType(arg); 
    }, data);

    data_.insert_or_assign(key, std::move(container));
}

void DataBase::SetTTL(const std::string& key, int64_t ttl) {
    DeleteExpiredKeys();

    auto it = data_.find(key);
    if (it == data_.end()) return;

    it->second.expires_ = Now() + ttl;
    ttl_[key] = it->second.expires_;
}

bool DataBase::IsKeyExist(const std::string& key) {
    DeleteExpiredKeys();

    if (IsKeyExpired(key)) {
        Delete(key);
        return false;
    }

    return (data_.find(key) != data_.end());
}

bool DataBase::IsKeyExpired(const std::string& key) {
    DeleteExpiredKeys();

    auto it = data_.find(key);

    if (it == data_.end()) {
        return false;
    }
    
    if (it->second.expires_ == -1) {
        return false;
    }

    return (it->second.expires_ <= Now());
}

int64_t DataBase::TimeLeftUntilExpired(const std::string& key) {
    DeleteExpiredKeys();

    auto it = data_.find(key);
    if (it == data_.end() || IsKeyExpired(key)) {
        return -2;
    }

    if (it->second.expires_ == -1) {
        return -1;
    }
    
    int64_t result = it->second.expires_ - Now();
    return (result > 0) ? result : 0;
}

void DataBase::DeleteExpiredKeys(bool force) {
    static int64_t last_time_used = 0;
    int64_t now = Now();

    if (!force && (now - last_time_used < kDBtimeDelta)) return;

    last_time_used = now;

    if (ttl_.empty()) return;

    const int kRandomKeysToChoose = 20;
    const int kMaxIterations = 100;

    for (int j = 0; j < kMaxIterations; ++j) {
        int expired = 0;
        int total = 0;

        for (int i = 0; i < kRandomKeysToChoose; ++i) {
            ++total;
            size_t bucket = generator_() % ttl_.bucket_count();

            if (ttl_.bucket_size(bucket) == 0) continue;

            auto it = ttl_.begin(bucket);
            auto least_time = it;

            for (; it != ttl_.end(bucket); ++it) {
                if (it->second < least_time->second) {
                    least_time = it;
                }
            }

            if (least_time->second <= now) {
                std::string key_delete = least_time->first;

                data_.erase(key_delete);
                ttl_.erase(key_delete);

                ++expired;
            }
        }

        if (expired * 4 < total) {
            break;
        }
    }
}
