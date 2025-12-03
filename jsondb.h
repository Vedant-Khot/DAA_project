#ifndef JSONDB_H
#define JSONDB_H

#include <string>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "Models.h"

using json = nlohmann::json;

// Graph Edge Structure (Represents a single flight connection)
struct Edge {
    std::string destination; // e.g., "BOM"
    int weight_minutes;      // e.g., 120
    std::string flight_id;   // "FL101"
    std::string date;        // "2025-12-01"
    std::string dep_time;    // "14:30"
    std::string arr_time;    // "16:30"
    int price;
    std::string airline;
};

class JsonDB {
private:
    std::string filename;
    json data;
    std::mutex db_mutex;

    // The Graph: Source Airport Code -> List of Outgoing Flights
    std::unordered_map<std::string, std::vector<Edge>> adj_list;

    void seed_data();
    void save();
    
    // Internal Helper to build the graph from JSON data
    void build_graph(); 
    // Internal Helper to convert "2h 15m" to 135 minutes
    int parse_duration_string(const std::string& dur);

public:
    JsonDB(const std::string& fname);

    // --- READ APIs ---
    json get_all_airports();
    json get_flights_limited(int limit);
    
    // THE NEW SMART ALGORITHM
    // Returns top K paths (Direct + Connecting) matching the date
    json find_smart_routes(const std::string& src, const std::string& dst, const std::string& date, int k = 5);

    // --- ADMIN APIs ---
    bool add_airport(const Airport& airport);
    bool delete_airport(const std::string& code);
    bool update_airport(const std::string& code, const json& new_data);

    bool add_flight(const Flight& flight);
    bool delete_flight(const std::string& id);
    bool update_flight(const std::string& id, const json& new_data);
};

#endif