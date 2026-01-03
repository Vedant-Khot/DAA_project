#ifndef JSONDB_H
#define JSONDB_H

#include <string>
#include <mutex>    // <--- REQUIRED for mutex
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "Models.h"

using json = nlohmann::json;

// Internal Edge Structure for Graph Algorithms
struct Edge {
    std::string destination; 
    int weight_minutes;      
    std::string flight_id;   
    std::string date;        
    std::string dep_time;    
    std::string arr_time;    
    int price;
    std::string airline;
};

class JsonDB {
private:
    std::string filename;
    json data;
    std::mutex db_mutex; // <--- REQUIRED: This is the variable causing your error

    // The Graph: Source Code -> List of Flights
    std::unordered_map<std::string, std::vector<Edge>> adj_list;

    void seed_data();
    void save();
    void build_graph(); 
    int parse_duration_string(const std::string& dur);

public:
    JsonDB(const std::string& fname);

    // Read APIs
    json get_all_airports();
    json get_flights_limited(int limit);
    
    // Smart Search
    json find_smart_routes(const std::string& src, const std::string& dst, const std::string& date, int k = 5);

    // Bellman-Ford Search (Single Best Path)
    json find_bellman_route(const std::string& src, const std::string& dst, const std::string& date);

    // Admin APIs
    bool add_airport(const Airport& airport);
    bool delete_airport(const std::string& code);
    bool update_airport(const std::string& code, const json& new_data);
    
    bool add_flight(const Flight& flight);
    bool delete_flight(const std::string& id);
    bool update_flight(const std::string& id, const json& new_data);

    // Booking APIs
    bool add_booking(const Booking& booking);
    json get_all_bookings();
    json get_booking_by_id(const std::string& booking_id);
    json get_bookings_by_email(const std::string& email);
    json get_bookings_by_user_id(const std::string& user_id);
    bool cancel_booking(const std::string& booking_id);
    // Admin Stats
    json get_admin_stats();

    // User management
    bool add_user(const User& user);
    json get_user_by_email(const std::string& email);
    json get_all_users();
};

#endif