#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ==============================
// 1. AIRPORT MODEL
// ==============================
struct Airport {
    int id;             // e.g., 1
    std::string code;   // e.g., "DEL"
    std::string name;   // e.g., "Indira Gandhi Intl"
    std::string city;   // e.g., "New Delhi"
    double lat;         // Latitude
    double lng;         // Longitude (Mapped to "long" in JSON)
};

// Custom JSON serializer to handle "long" keyword
inline void to_json(json& j, const Airport& a) {
    j = json{
        {"id", a.id},
        {"code", a.code},
        {"name", a.name},
        {"city", a.city},
        {"lat", a.lat},
        {"long", a.lng} 
    };
}

inline void from_json(const json& j, Airport& a) {
    j.at("id").get_to(a.id);
    j.at("code").get_to(a.code);
    j.at("name").get_to(a.name);
    j.at("city").get_to(a.city);
    j.at("lat").get_to(a.lat);
    j.at("long").get_to(a.lng);
}

// ==============================
// 2. FLIGHT MODEL
// ==============================
struct Flight {
    std::string id;         // e.g., "FL1001"
    std::string airline;    // e.g., "IndiGo"
    std::string from_code;  // e.g., "DEL"
    std::string to_code;    // e.g., "BOM"
    std::string date;       // e.g., "2025-12-01"
    std::string departure;  // e.g., "14:30"
    std::string arrival;    // e.g., "16:45"
    std::string duration;   // e.g., "2h 15m"
    int price;              // e.g., 4500

    // Automatic JSON Conversion
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Flight, id, airline, from_code, to_code, date, departure, arrival, duration, price)
};

#endif