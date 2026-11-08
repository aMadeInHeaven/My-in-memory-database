#pragma once

#include <vector>
#include <set>
#include <string>
#include <list>
#include <optional>
#include <variant>

#include "../HelpingStructures.h"

struct NullType {};

struct StatusReturnType {
    StatusReturnType() : status("") {}
    StatusReturnType(const std::string& s) : status(s) {}
    StatusReturnType(std::string&& s) : status(std::move(s)) {}

    StatusReturnType(const StatusReturnType& s) = default;
    StatusReturnType& operator=(const StatusReturnType& s) = default;
    StatusReturnType(StatusReturnType&& s) = default;
    StatusReturnType& operator=(StatusReturnType&& s) = default;

    std::string status;
};

struct IntegerReturnType {
    IntegerReturnType() : data(0) {}
    IntegerReturnType(long long s) : data(s) {}
    long long data;
};

struct StringReturnType {
    StringReturnType() : data("") {}
    StringReturnType(const std::string& s) : data(s) {}
    StringReturnType(std::string&& s) : data(std::move(s)) {}

    StringReturnType(const StringReturnType& s) = default;
    StringReturnType& operator=(const StringReturnType& s) = default;
    StringReturnType(StringReturnType&& s) = default;
    StringReturnType& operator=(StringReturnType&& s) = default;

    std::string data;
};

struct ArrayReturnType {
    ArrayReturnType() : data({}) {}
    ArrayReturnType(const std::vector<std::string>& s) : data(s) {}
    ArrayReturnType(std::vector<std::string>&& s) : data(std::move(s)) {}

    ArrayReturnType(const ArrayReturnType& s) = default;
    ArrayReturnType& operator=(const ArrayReturnType& s) = default;
    ArrayReturnType(ArrayReturnType&& s) = default;
    ArrayReturnType& operator=(ArrayReturnType&& s) = default;

    std::vector<std::string> data;
};

struct SetReturnType {
    SetReturnType() : data({}) {}
    SetReturnType(const std::vector<std::string>& s) : data(s) {}
    SetReturnType(std::vector<std::string>&& s) : data(std::move(s)) {}

    SetReturnType(const SetReturnType& s) = default;
    SetReturnType& operator=(const SetReturnType& s) = default;
    SetReturnType(SetReturnType&& s) = default;
    SetReturnType& operator=(SetReturnType&& s) = default;

    std::vector<std::string> data;
};

struct GeoReturnType {
    struct PointData {
        std::string name;
        double lat;
        double lon;
    };

    std::vector<PointData> points;
};

struct GeoArrayReturnType {
    GeoArrayReturnType() : data({}) {}
    GeoArrayReturnType(const std::vector<std::optional<Point>>& s) : data(s) {}
    GeoArrayReturnType(std::vector<std::optional<Point>>&& s) : data(std::move(s)) {}

    GeoArrayReturnType(const GeoArrayReturnType& s) = default;
    GeoArrayReturnType& operator=(const GeoArrayReturnType& s) = default;
    GeoArrayReturnType(GeoArrayReturnType&& s) = default;
    GeoArrayReturnType& operator=(GeoArrayReturnType&& s) = default;

    std::vector<std::optional<Point>> data;
};

struct ErrorReturnType {
    ErrorReturnType() : error_message("") {}
    ErrorReturnType(const std::string& s) : error_message(s) {}
    ErrorReturnType(std::string&& s) : error_message(std::move(s)) {}

    ErrorReturnType(const ErrorReturnType& s) = default;
    ErrorReturnType& operator=(const ErrorReturnType& s) = default;
    ErrorReturnType(ErrorReturnType&& s) = default;
    ErrorReturnType& operator=(ErrorReturnType&& s) = default;

    std::string error_message;
};

using ReturnTypesVariant = std::variant<
                                NullType, 
                                StatusReturnType, 
                                IntegerReturnType, 
                                StringReturnType, 
                                ArrayReturnType,
                                SetReturnType,
                                GeoReturnType,
                                GeoArrayReturnType, 
                                ErrorReturnType
                            >;
