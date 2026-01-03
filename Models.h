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

// ==============================
// 3. BOOKING MODEL
// ==============================
struct Booking {
    std::string booking_id;        // e.g., "BK12345678"
    std::string user_id;           // Unique user identifier from localStorage
    std::string flight_id;         // Reference to flight ID
    std::string passenger_name;    // e.g., "John Doe"
    std::string passenger_email;   // e.g., "john@example.com"
    std::string from_code;         // e.g., "DEL"
    std::string to_code;           // e.g., "BOM"
    std::string date;              // e.g., "2025-12-01"
    int total_price;               // Total amount paid
    std::string booking_date;      // When booking was made
    std::string status;            // e.g., "confirmed", "cancelled"

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Booking, booking_id, user_id, flight_id, passenger_name, passenger_email, 
                                   from_code, to_code, date, total_price, booking_date, status)
};

// ==============================
// 4. USER MODEL
// ==============================
struct User {
    std::string id;
    std::string name;
    std::string email;
    std::string password; // Plain text for now, should be hashed in production
    std::string created_at;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(User, id, name, email, password, created_at)
};

#endif