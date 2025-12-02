#include "JsonDB.h"
#include <fstream>
#include <iostream>
#include <cstdlib> // For random numbers

// Constructor
JsonDB::JsonDB(const std::string& fname) : filename(fname) {
    std::ifstream file(filename);
    if (file.is_open()) {
        try { file >> data; } catch (...) { data = json::object(); }
    }
    // If file is empty or missing data, generate it
    if (data.empty() || !data.contains("airports")) {
        seed_data();
    }
}

void JsonDB::save() {
    std::ofstream file(filename);
    file << data.dump(4);
}

// ==========================================
// SEEDING LOGIC (The Heavy Lifter)
// ==========================================
void JsonDB::seed_data() {
    std::cout << "[INFO] Seeding database with 50 Airports and Flights..." << std::endl;

    // 1. The 50 Indian Airports
    std::vector<Airport> airports = {
        {1, "DEL", "Indira Gandhi Intl", "New Delhi", 28.5562, 77.1000},
        {2, "BOM", "Chhatrapati Shivaji Maharaj Intl", "Mumbai", 19.0896, 72.8656},
        {3, "BLR", "Kempegowda Intl", "Bengaluru", 13.1986, 77.7066},
        {4, "MAA", "Chennai Intl", "Chennai", 12.9941, 80.1709},
        {5, "CCU", "Netaji Subhas Chandra Bose Intl", "Kolkata", 22.6547, 88.4467},
        {6, "HYD", "Rajiv Gandhi Intl", "Hyderabad", 17.2403, 78.4294},
        {7, "COK", "Cochin Intl", "Kochi", 10.1518, 76.3930},
        {8, "AMD", "Sardar Vallabhbhai Patel Intl", "Ahmedabad", 23.0732, 72.6347},
        {9, "PNQ", "Pune Intl", "Pune", 18.5821, 73.9197},
        {10, "GOI", "Dabolim", "Goa", 15.3800, 73.8314},
        {11, "TRV", "Thiruvananthapuram Intl", "Thiruvananthapuram", 8.4821, 76.9200},
        {12, "CCJ", "Calicut Intl", "Kozhikode", 11.1363, 75.9553},
        {13, "LKO", "Chaudhary Charan Singh Intl", "Lucknow", 26.7606, 80.8893},
        {14, "GAU", "Lokpriya Gopinath Bordoloi Intl", "Guwahati", 26.1061, 91.5859},
        {15, "JAI", "Jaipur Intl", "Jaipur", 26.8289, 75.8056},
        {16, "SXR", "Srinagar Intl", "Srinagar", 33.9876, 74.7741},
        {17, "BBI", "Biju Patnaik Intl", "Bhubaneswar", 20.2444, 85.8178},
        {18, "PAT", "Jay Prakash Narayan Intl", "Patna", 25.5913, 85.0880},
        {19, "IXC", "Chandigarh Intl", "Chandigarh", 30.6735, 76.7885},
        {20, "IXB", "Bagdogra Intl", "Bagdogra", 26.6812, 88.3286},
        {21, "IDR", "Devi Ahilya Bai Holkar", "Indore", 22.7217, 75.8011},
        {22, "NGP", "Dr. Babasaheb Ambedkar Intl", "Nagpur", 21.0922, 79.0472},
        {23, "VNS", "Lal Bahadur Shastri Intl", "Varanasi", 25.4497, 82.8537},
        {24, "ATQ", "Sri Guru Ram Dass Jee Intl", "Amritsar", 31.7096, 74.7973},
        {25, "VTZ", "Visakhapatnam Intl", "Visakhapatnam", 17.7211, 83.2245},
        {26, "RPR", "Swami Vivekananda", "Raipur", 21.1804, 81.7388},
        {27, "IXM", "Madurai", "Madurai", 9.8345, 78.0934},
        {28, "CJB", "Coimbatore Intl", "Coimbatore", 11.0295, 77.0434},
        {29, "IXR", "Birsa Munda", "Ranchi", 23.3143, 85.3217},
        {30, "UDR", "Maharana Pratap", "Udaipur", 24.6172, 73.8962},
        {31, "BDQ", "Vadodara", "Vadodara", 22.3360, 73.2263},
        {32, "JGA", "Jamnagar", "Jamnagar", 22.4665, 70.0125},
        {33, "IXL", "Kushok Bakula Rimpochee", "Leh", 34.1359, 77.5465},
        {34, "TRZ", "Tiruchirappalli Intl", "Tiruchirappalli", 10.7654, 78.7097},
        {35, "IXJ", "Jammu", "Jammu", 32.6891, 74.8375},
        {36, "BHO", "Raja Bhoj", "Bhopal", 23.2875, 77.3378},
        {37, "JDH", "Jodhpur", "Jodhpur", 26.2515, 73.0485},
        {38, "IXA", "Agartala", "Agartala", 23.8870, 91.2404},
        {39, "IMF", "Imphal", "Imphal", 24.7600, 93.8967},
        {40, "STV", "Surat", "Surat", 21.1137, 72.7418},
        {41, "IXE", "Mangaluru Intl", "Mangaluru", 12.9613, 74.8901},
        {42, "TIR", "Tirupati", "Tirupati", 13.6325, 79.5436},
        {43, "VGA", "Vijayawada", "Vijayawada", 16.5304, 80.7968},
        {44, "IXZ", "Veer Savarkar Intl", "Port Blair", 11.6410, 92.7297},
        {45, "DED", "Dehradun", "Dehradun", 30.1897, 78.1803},
        {46, "HBX", "Hubli", "Hubli", 15.3617, 75.0849},
        {47, "AJL", "Lengpui", "Aizawl", 23.8397, 92.6236},
        {48, "DMU", "Dimapur", "Dimapur", 25.8839, 93.7714},
        {49, "MYQ", "Mysuru", "Mysuru", 12.2300, 76.6500},
        {50, "GWL", "Gwalior", "Gwalior", 26.2936, 78.2274}
    };
    data["airports"] = airports;

    // 2. Generate 250+ Flights
    std::vector<Flight> flights;
    int flight_counter = 1000;
    std::string airlines[] = {"IndiGo", "Air India", "Vistara", "SpiceJet", "Akasa Air"};

    for (size_t i = 0; i < airports.size(); ++i) {
        std::string src = airports[i].code;

        // Create 5 flights per airport
        for (int j = 1; j <= 5; ++j) {
            int dest_index = (i + j) % airports.size();
            std::string dst = airports[dest_index].code;

            // Randomize Data
            int airline_idx = rand() % 5;
            int dep_hour = 6 + (rand() % 16); // 06:00 to 22:00
            int dep_min = (rand() % 4) * 15;  // 00, 15, 30, 45
            int dur_hour = 1 + (rand() % 3);  // 1 to 3 hours
            int arr_hour = (dep_hour + dur_hour) % 24;

            // Format Time Strings
            char time_buf[10];
            sprintf(time_buf, "%02d:%02d", dep_hour, dep_min);
            std::string departure = time_buf;
            
            sprintf(time_buf, "%02d:%02d", arr_hour, dep_min);
            std::string arrival = time_buf;

            std::string duration = std::to_string(dur_hour) + "h 00m";
            int price = 3000 + (rand() % 5000); // 3000 to 8000

            Flight f;
            f.id = "FL" + std::to_string(flight_counter++);
            f.airline = airlines[airline_idx];
            f.from_code = src;
            f.to_code = dst;
            f.date = "2025-12-" + std::to_string(10 + j);
            f.departure = departure;
            f.arrival = arrival;
            f.duration = duration;
            f.price = price;

            flights.push_back(f);
        }
    }
    data["flights"] = flights;
    save();
}

// --- READ OPERATIONS ---
json JsonDB::get_all_airports() {
    std::lock_guard<std::mutex> lock(db_mutex);
    return data.value("airports", json::array());
}

json JsonDB::get_flights_limited(int limit) {
    std::lock_guard<std::mutex> lock(db_mutex);
    json result = json::array();
    if (!data.contains("flights")) return result;
    
    int count = 0;
    for (const auto& f : data["flights"]) {
        if (count++ >= limit) break;
        result.push_back(f);
    }
    return result;
}

json JsonDB::search_flights(const std::string& src, const std::string& dst) {
    std::lock_guard<std::mutex> lock(db_mutex);
    json result = json::array();
    if (!data.contains("flights")) return result;

    for (const auto& f : data["flights"]) {
        if (f.value("from_code", "") == src && f.value("to_code", "") == dst) {
            result.push_back(f);
        }
    }
    return result;
}

json JsonDB::search_flights_by_date(const std::string& date) {
    std::lock_guard<std::mutex> lock(db_mutex);
    json result = json::array();
    if (!data.contains("flights")) return result;

    for (const auto& f : data["flights"]) {
        if (f.value("date", "") == date) result.push_back(f);
    }
    return result;
}

// --- ADMIN OPERATIONS (Airports) ---
bool JsonDB::add_airport(const Airport& apt) {
    std::lock_guard<std::mutex> lock(db_mutex);
    if (!data.contains("airports")) data["airports"] = json::array();
    
    for (const auto& existing : data["airports"]) {
        if (existing.value("code", "") == apt.code) return false; // Duplicate
    }
    
    // Explicitly convert struct to json before pushing
    json j_apt = apt; 
    data["airports"].push_back(j_apt);
    save();
    return true;
}

bool JsonDB::delete_airport(const std::string& code) {
    std::lock_guard<std::mutex> lock(db_mutex);
    if (!data.contains("airports")) return false;
    
    auto& arr = data["airports"];
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        if ((*it).value("code", "") == code) {
            arr.erase(it);
            save();
            return true;
        }
    }
    return false;
}

bool JsonDB::update_airport(const std::string& code, const json& new_data) {
    std::lock_guard<std::mutex> lock(db_mutex);
    if (!data.contains("airports")) return false;

    for (auto& apt : data["airports"]) {
        if (apt.value("code", "") == code) {
            for (auto& el : new_data.items()) apt[el.key()] = el.value();
            save();
            return true;
        }
    }
    return false;
}

// --- ADMIN OPERATIONS (Flights) ---
bool JsonDB::add_flight(const Flight& fl) {
    std::lock_guard<std::mutex> lock(db_mutex);
    if (!data.contains("flights")) data["flights"] = json::array();

    for (const auto& existing : data["flights"]) {
        if (existing.value("id", "") == fl.id) return false;
    }

    json j_fl = fl;
    data["flights"].push_back(j_fl);
    save();
    return true;
}

bool JsonDB::delete_flight(const std::string& id) {
    std::lock_guard<std::mutex> lock(db_mutex);
    if (!data.contains("flights")) return false;

    auto& arr = data["flights"];
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        if ((*it).value("id", "") == id) {
            arr.erase(it);
            save();
            return true;
        }
    }
    return false;
}

bool JsonDB::update_flight(const std::string& id, const json& new_data) {
    std::lock_guard<std::mutex> lock(db_mutex);
    if (!data.contains("flights")) return false;

    for (auto& fl : data["flights"]) {
        if (fl.value("id", "") == id) {
            for (auto& el : new_data.items()) fl[el.key()] = el.value();
            save();
            return true;
        }
    }
    return false;
}