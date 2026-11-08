#pragma once

#include <variant>
#include <string>
#include <vector>
#include <list>
#include <cstdint>
#include <utility>
#include <set>
#include <cmath>
#include <unordered_map>
#include <deque>
#include <scoped_allocator>
#include <iostream>

#include "../ReturnTypes/ReturnTypes.h"
#include "../HelpingStructures.h"

const size_t kListNodeSize = 32;
const size_t kSetNodeSize = 48;
const size_t kMapNodeSize = 48;

struct AllocatorMemoryBlock {
    static inline size_t current_memory = 0;
    static inline size_t max_memory = SIZE_MAX;

    static bool CanAllocate(size_t n) {
        return current_memory + n <= max_memory;
    }
};

template <class T>
class DBAllocatorForTypes {
public:
    using value_type = T;
    using pointer = T*;

    DBAllocatorForTypes() = default;

    template <class Other>
    DBAllocatorForTypes(const DBAllocatorForTypes<Other>&) {}

    pointer allocate(size_t n) {
        size_t want_memory = n * sizeof(value_type);

        if (!AllocatorMemoryBlock::CanAllocate(want_memory)) {
            throw std::bad_alloc();
        }

        pointer memory_allocated = static_cast<pointer>(::operator new(want_memory));
        
        if (!memory_allocated) {
            throw std::bad_alloc();
        }

        AllocatorMemoryBlock::current_memory += want_memory;

        return memory_allocated;
    }

    void deallocate(pointer ptr, size_t n) {
        size_t deleting_memory = n * sizeof(T);
        
        if (AllocatorMemoryBlock::current_memory >= deleting_memory) {
            AllocatorMemoryBlock::current_memory -= deleting_memory;
        } else {
            AllocatorMemoryBlock::current_memory = 0;
        }
        
        ::operator delete(ptr);
    }

    template <typename Other>
    struct rebind {
        using other = DBAllocatorForTypes<Other>;
    };

    bool operator==(const DBAllocatorForTypes&) const { 
        return true; 
    }

    bool operator!=(const DBAllocatorForTypes&) const { 
        return false; 
    }
};

template <class T>
using Allocator = std::scoped_allocator_adaptor<DBAllocatorForTypes<T>>;

using DBstring = std::basic_string<char, std::char_traits<char>, DBAllocatorForTypes<char>>;

template <class T>
using DBlist = std::deque<T, Allocator<T>>;

template<typename T>
using DBset = std::set<T, std::less<T>, Allocator<T>>;

template<typename K, typename V>
using DBmap = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, Allocator<std::pair<const K, V>>>;

struct NoneType {
    size_t MemoryUsed() const {
        return 0;
    }

    NullType ReturnTypeResult() const {
        return NullType();
    }
};

struct StringType {
    DBstring data;

    StringType() = default;
    StringType(const std::string& str) : data(str.c_str()) {}
    StringType(std::string&& str) : data(str.c_str()) {}

    void Append(const std::string& new_part) {
        data.append(new_part.c_str());
    }

    size_t Len() const { 
        return data.size(); 
    }

    size_t MemoryUsed() const {
        return data.capacity() + sizeof(StringType);
    }

    StringReturnType ReturnTypeResult() const {
        return StringReturnType(std::string(data.c_str()));
    }
};

struct ListType {
    DBlist<DBstring> data;

    ListType() : data(Allocator<DBstring>{}) {}

    void PushBack(const std::string& value) {
        data.emplace_back(value.c_str());
    }

    void PushFront(const std::string& value) {
        data.emplace_front(value.c_str());
    }

    StringReturnType PopBack() {
        if (data.empty()) {
            return StringReturnType();
        }

        StringReturnType result(std::string(data.back().c_str()));
        data.pop_back();
        return result;
    }

    StringReturnType PopFront() {
        if (data.empty()) {
            return StringReturnType();
        }

        StringReturnType result(std::string(data.front().c_str()));
        data.pop_front();
        return result;
    }

    size_t Len() const {
        return data.size();
    }

    bool Empty() const {
        return data.empty();
    }

    void SetIndex(int index, const std::string& value) {
        if (index >= data.size()) {
            return;
        }

        data[index] = value.c_str();
    }

    size_t MemoryUsed() const {
        size_t result = sizeof(ListType);

        for (const auto& elem : data) {
            result += elem.capacity() + kListNodeSize;
        }

        return result;
    }

    ArrayReturnType ReturnTypeResult() const {
        std::vector<std::string> result;
        for(const auto& s : data) {
            result.push_back(s.c_str());
        }

        return ArrayReturnType(std::move(result));
    }
};

struct SetType {
    DBset<DBstring> data;

    SetType() : data(Allocator<DBstring>{}) {}

    void Add(const std::string& value) {
        data.emplace(value.c_str());
    }

    void Remove(const std::string& value) {
        auto it = data.find(value.c_str());

        if (it == data.end()) {
            return;
        }

        data.erase(it);
    }

    size_t Len() const { 
        return data.size(); 
    }

    size_t MemoryUsed() const {
        size_t result = sizeof(SetType);

        for (const auto& elem : data) {
            result += elem.capacity() + kSetNodeSize;
        }

        return result;
    }

    ArrayReturnType ReturnTypeResult() const {
        std::vector<std::string> result;
        for(const auto& s : data) {
            result.push_back(std::string(s.c_str()));
        }

        return ArrayReturnType(std::move(result));
    }
};

struct GeoType {
    DBmap<DBstring, Point> data;

    GeoType() : data(Allocator<std::pair<const DBstring, Point>>{}) {}

    void AddPoint(const std::string& name, double lat, double lon) {
        data.insert_or_assign(DBstring(name.c_str()), Point{lat, lon});
    }

    void RemovePoint(const std::string& name) {
        auto it = data.find(name.c_str());
        if (it == data.end()) {
            return;
        }

        data.erase(it);
    }

    static double EverythingToKm(const std::string& unit) {
        if (unit == "km") {
            return 1.0;
        }
        if (unit == "m") {
            return 1000.0;
        }
        if (unit == "mi") {
            return 0.621371;
        }
        if (unit == "ft") {
            return 3280.84;
        }
        return 1000.0;
    }

    static double Haversine(double lat1, double lon1, double lat2, double lon2) {
        static const double R = 6372.8;
        static const double PI = 3.1415926;
        
        auto toRad = [](double degree) { return degree * PI / 180.0; };
        
        double phi1 = toRad(lat1);
        double phi2 = toRad(lat2);
        double dPhi = toRad(lat2 - lat1);
        double dLambda = toRad(lon2 - lon1);

        double a = std::sin(dPhi / 2) * std::sin(dPhi / 2) +
                std::cos(phi1) * std::cos(phi2) *
                std::sin(dLambda / 2) * std::sin(dLambda / 2);
                
        double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

        return R * c;
    }

    size_t MemoryUsed() const {
        size_t result = sizeof(GeoType);

        for (auto it = data.begin(); it != data.end(); ++it) {
            result += it->first.capacity() + sizeof(Point) + kMapNodeSize;
        }

        return result;
    }

    GeoArrayReturnType ReturnTypeResult() const {
        std::vector<std::optional<Point>> result;
        
        result.reserve(data.size());

        for (auto it = data.begin(); it != data.end(); ++it) {
            result.push_back(it->second);
        }

        return GeoArrayReturnType(std::move(result));
    }
};

struct ValueContainer {
    using VariantType = std::variant<NoneType, StringType, ListType, SetType, GeoType>;

    ValueContainer() : data_(NoneType{}), expires_(-1) {}

    template <class T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, ValueContainer>>>
    ValueContainer(T&& value) : data_(std::forward<T>(value)), expires_(-1) {}

    VariantType data_;
    int64_t expires_;

    size_t MemoryUsed() const {
        return std::visit([](auto&& arg) { 
            return arg.MemoryUsed(); 
        }, data_);
    }

    static ValueContainer ConstructFromReturnType(const ReturnTypesVariant& ret_type) {
        return std::visit(OverloadPattern {
            [](const NullType&) -> ValueContainer {
                return ValueContainer();
            },
            [](const StringReturnType& s) -> ValueContainer {
                return ValueContainer(StringType(s.data));
            },
            [](const ArrayReturnType& a) -> ValueContainer {
                ListType l;
                for (const auto& s : a.data) {
                    l.PushBack(s);
                }
                return ValueContainer(std::move(l));
            },
            [](const SetReturnType& s) -> ValueContainer {
                SetType set_obj;
                for (const auto& value : s.data) {
                    set_obj.Add(value);
                }

                return ValueContainer(std::move(set_obj));
            },
            [](const IntegerReturnType& i) -> ValueContainer {
                return ValueContainer(StringType(std::to_string(i.data)));
            },
            [](const StatusReturnType& s) -> ValueContainer {
                return ValueContainer(StringType(s.status));
            },
            [](const ErrorReturnType& e) -> ValueContainer {
                return ValueContainer(StringType(e.error_message));
            },
            [](const GeoReturnType& g) -> ValueContainer {
                GeoType geo_obj;

                for (const auto& p : g.points) {
                    geo_obj.AddPoint(p.name, p.lat, p.lon);
                }
                return ValueContainer(std::move(geo_obj));
            },
            [](const GeoArrayReturnType&) -> ValueContainer {
                return ValueContainer();
            }
        }, ret_type);
    }
};
