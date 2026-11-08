#include "GeoCommands.h"
#include "../Engine/DataBase.h"
#include "../ValueObject/Value.h"
#include "../HelpingStructures.h"

#include <vector>
#include <variant>
#include <string>
#include <algorithm>


InterfaceCommand::ExecuteType GEOADDgeo::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 5 || (tokens.size() - 2) % 3 != 0) {
        return ErrorReturnType("wrong number of arguments for 'geoadd' command");
    }

    auto geo_post_ptr = db.GetPtr(tokens[1]);
    if (!geo_post_ptr) {
        db.Set(tokens[1], GeoReturnType());
        geo_post_ptr = db.GetPtr(tokens[1]);
    }

    auto* geo = std::get_if<GeoType>(&geo_post_ptr->data_);
    if (!geo) {
        return ErrorReturnType("wrongtype operation against a key holding the wrong kind of value");
    }

    long long added = 0;
    try {
        for (size_t i = 2; i < tokens.size(); i += 3) {
            double lon = std::stod(tokens[i]);
            double lat = std::stod(tokens[i+1]);

            geo->AddPoint(tokens[i+2], lat, lon);
            ++added;
        }
    } catch (...) { 
        return ErrorReturnType("value must be a float"); 
    }

    return IntegerReturnType(added);
}

InterfaceCommand::ExecuteType GEOPOSgeo::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 3) {
        return ErrorReturnType("wrong number of arguments for 'geopos' command");
    }
    
    auto geo_post_ptr = db.GetPtr(tokens[1]);

    if (!geo_post_ptr) {
        return NullType();
    }

    auto* geo = std::get_if<GeoType>(&geo_post_ptr->data_);
    if (!geo) {
        return ErrorReturnType("wrongtype operation against a key holding the wrong kind of value");
    }

    std::vector<std::optional<Point>> coords;
    for (size_t i = 2; i < tokens.size(); ++i) {
        auto it = geo->data.find(DBstring(tokens[i].c_str()));

        if (it != geo->data.end()) {
            coords.push_back(Point{it->second.lat, it->second.lon});
        } else {
            coords.push_back(std::nullopt);
        }
    }

    return GeoArrayReturnType(coords);
}

InterfaceCommand::ExecuteType GEODISTgeo::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 4 || tokens.size() > 5) {
        return ErrorReturnType("wrong number of arguments for 'geodist' command");
    }

    auto geo_post_ptr = db.GetPtr(tokens[1]);
    if (!geo_post_ptr) {
        return NullType();
    }

    auto* geo = std::get_if<GeoType>(&geo_post_ptr->data_);
    if (!geo) {
        return ErrorReturnType("wrongtype operation against a key holding the wrong kind of value");
    }

    auto it1 = geo->data.find(DBstring(tokens[2].c_str()));
    auto it2 = geo->data.find(DBstring(tokens[3].c_str()));
    if (it1 == geo->data.end() || it2 == geo->data.end()) {
        return NullType();
    }

    double dist = GeoType::Haversine(it1->second.lat, it1->second.lon, it2->second.lat, it2->second.lon);
    
    std::string unit = (tokens.size() == 5) ? tokens[4] : "m";
    dist *= GeoType::EverythingToKm(unit);

    return StringReturnType(std::to_string(dist));
}


struct GeoResult {
    std::string name;
    double distance;
};

std::vector<GeoResult> SearchForPoints(const GeoType& geo, const std::vector<std::string>& tokens, size_t start_idx, std::string& error_msg) {
    try {
        double lon = std::stod(tokens[start_idx + 1]);
        double lat = std::stod(tokens[start_idx + 2]);
        double radius = std::stod(tokens[start_idx + 4]);
        std::string unit = tokens[start_idx + 5];
        double multiplier = GeoType::EverythingToKm(unit);

        std::vector<GeoResult> found;
        for (const auto& [name, point] : geo.data) {
            double dist = GeoType::Haversine(lat, lon, point.lat, point.lon);
            double converted_dist = dist * multiplier;

            if (converted_dist <= radius) {
                found.push_back({std::string(name.c_str()), converted_dist});
            }
        }

        bool asc = false, desc = false;
        size_t count = 0;

        for (size_t i = start_idx + 6; i < tokens.size(); ++i) {
            std::string opt = tokens[i];
            std::transform(opt.begin(), opt.end(), opt.begin(), [](unsigned char c) {
                return std::toupper(c);
            });
            
            if (opt == "ASC") {
                asc = true;
            } else if (opt == "DESC") {
                desc = true;
            } else if (opt == "COUNT" && i + 1 < tokens.size()) {
                count = std::stoul(tokens[++i]);
            }
        }

        if (asc) {
            std::sort(found.begin(), found.end(), [](auto& a, auto& b) { return a.distance < b.distance; });
        } else if (desc) {
            std::sort(found.begin(), found.end(), [](auto& a, auto& b) { return a.distance > b.distance; });
        }

        if (count > 0 && count < found.size()) {
            found.resize(count);
        }

        return found;
    } catch (...) {
        error_msg = "value must be a float";
        return {};
    }
}

InterfaceCommand::ExecuteType GEOSEARCHgeo::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 8 || tokens.size() > 11) {
        return ErrorReturnType("wrong number of arguments for 'geosearch' command");
    }

    auto geo_post_ptr = db.GetPtr(tokens[1]);
    if (!geo_post_ptr) {
        return NullType();
    }

    auto* geo = std::get_if<GeoType>(&geo_post_ptr->data_);
    if (!geo) {
        return ErrorReturnType("wrongtype operation against a key holding the wrong kind of value");
    }

    std::string error;
    auto results = SearchForPoints(*geo, tokens, 2, error);
    if (!error.empty()) {
        return ErrorReturnType(error);
    }

    std::vector<std::string> names;
    for (const auto& r : results) {
        std::string data;
        data += r.name + '\n' + "distance: " + std::to_string(r.distance) + '\n' + 
            "lon: " + std::to_string(geo->data[DBstring(r.name)].lon) + '\n' + "lat: " + std::to_string(geo->data[DBstring(r.name)].lat);
        names.push_back(data);   
    }

    return ArrayReturnType(names);
}

InterfaceCommand::ExecuteType GEOSEARCHSTOREgeo::Execute(DataBase& db, const std::vector<std::string>& tokens) {
    if (tokens.size() < 9 || tokens.size() > 11) {
        return ErrorReturnType("wrong number of arguments for 'geosearchstore' command");
    }

    auto source = db.GetPtr(tokens[2]);
    if (!source) {
        return IntegerReturnType(0);
    }

    auto* geo = std::get_if<GeoType>(&source->data_);
    if (!geo) {
        return ErrorReturnType("wrongtype operation against a key holding the wrong kind of value");
    }

    std::string error;
    auto results = SearchForPoints(*geo, tokens, 3, error);
    if (!error.empty()) {
        return ErrorReturnType(error);
    }

    std::vector<std::string> names;
    for (const auto& r : results) {
        names.push_back(r.name);
    }

    db.Set(tokens[1], ArrayReturnType(names));

    return IntegerReturnType(names.size());
}
