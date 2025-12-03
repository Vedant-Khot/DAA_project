#include <crow.h>
#include <nlohmann/json.hpp>
#include "jsondb.h"
#include "Models.h"
#include <iostream>

using json = nlohmann::json;


// --- Struct for K-Shortest Path ---
struct PathState {
    int total_time;
    string current_node;
    vector<string> path_history;

    // Min-heap comparator (Smallest time on top)
    bool operator>(const PathState& other) const {
        return total_time > other.total_time;
    }
};

// --- K-Shortest Paths Algorithm ---
vector<pair<int, vector<string>>> findKShortestPaths(const string& start, const string& end, int k = 5) {
    vector<pair<int, vector<string>>> results;
    
    // Priority Queue stores: {Cost, CurrentNode, PathSoFar}
    priority_queue<PathState, vector<PathState>, greater<PathState>> pq;
    
    // Count how many times we visited a node to prevent infinite cycles 
    // and prune unnecessary long paths.
    unordered_map<string, int> visit_counts;

    // Initialize
    pq.push({0, start, {start}});

    while (!pq.empty() && results.size() < k) {
        PathState top = pq.top();
        pq.pop();

        string u = top.current_node;
        int cost = top.total_time;
        vector<string> history = top.path_history;

        // If we reached destination, add to results
        if (u == end) {
            results.push_back({cost, history});
            continue; // Continue to find next best path
        }

        // Optimization: Don't process the same node more than K times
        // This effectively finds the 1st shortest, 2nd shortest... Kth shortest way to 'u'
        if (visit_counts[u] >= k) continue;
        visit_counts[u]++;

        // Explore Neighbors
        if (adj_list.find(u) != adj_list.end()) {
            for (const auto& edge : adj_list[u]) {
                string v = edge.destination;
                int new_cost = cost + edge.weight;

                // Cycle Prevention: Don't go back to a node already in THIS specific path history
                bool cycle = false;
                for (const string& node : history) {
                    if (node == v) { cycle = true; break; }
                }
                if (cycle) continue;

                // Create new path history
                vector<string> new_history = history;
                new_history.push_back(v);

                pq.push({new_cost, v, new_history});
            }
        }
    }

    return results;
}
// ==========================================
// 1. DEFINE CORS MIDDLEWARE
// This tells the browser: "It is okay to accept data from this server"
// ==========================================

struct CORSHandler {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        // No action needed before handling request
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        // Add these headers to EVERY response sent by the server
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }
};

// Initialize Database
JsonDB db("flight_database.json");

int main() {
    // CHANGE: Use App<CORSHandler> instead of SimpleApp
    crow::App<CORSHandler> app;

    // ==========================================
    // CORS is handled via CORSHandler middleware - no need for explicit OPTIONS routes
    // ==========================================

    // ==========================================
    // ROOT ENDPOINT
    // ==========================================
    CROW_ROUTE(app, "/")
    ([](){
        json response = {
            {"status", "ok"},
            {"message", "Flight Booking API Server"},
            {"version", "1.0.0"},
            {"endpoints", {
                {"GET /api/airports", "Get all airports"},
                {"GET /api/flights", "Get flights (limit: 10 by default)"},
                {"GET /api/search?from=X&to=Y", "Search flights"},
                {"GET /api/search_date?date=YYYY-MM-DD", "Search flights by date"},
                {"/health", "Health check endpoint"}
            }}
        };
        return crow::response(response.dump());
    });

    // ==========================================
    // HEALTH CHECK ENDPOINT
    // ==========================================
    CROW_ROUTE(app, "/health")
    ([](){
        return crow::response(200, "OK");
    });

    // ==========================================
    // PUBLIC ROUTES
    // ==========================================
    
    CROW_ROUTE(app, "/api/airports")
    ([](){
        return crow::response(db.get_all_airports().dump());
    });

    CROW_ROUTE(app, "/api/flights")
    ([](const crow::request& req){
        int limit = 10;
        if (req.url_params.get("limit")) limit = std::stoi(req.url_params.get("limit"));
        return crow::response(db.get_flights_limited(limit).dump());
    });

    CROW_ROUTE(app, "/api/search")
    ([](const crow::request& req){
        const char* src = req.url_params.get("from");
        const char* dst = req.url_params.get("to");
        if (!src || !dst) return crow::response(400, "Missing 'from' or 'to'");
        return crow::response(db.search_flights(src, dst).dump());
    });

    CROW_ROUTE(app, "/api/search_date")
    ([](const crow::request& req){
        const char* date = req.url_params.get("date");
        if (!date) return crow::response(400, "Missing 'date'");
        return crow::response(db.search_flights_by_date(date).dump());
    });

    // 3. SMART ROUTING SEARCH (New!)
    // URL: /api/search_smart?from=DEL&to=BLR&date=2025-12-01
    CROW_ROUTE(app, "/api/search_smart")
    ([](const crow::request& req){
        const char* src = req.url_params.get("from");
        const char* dst = req.url_params.get("to");
        const char* date = req.url_params.get("date"); // e.g. "2025-12-01"

        if (!src || !dst || !date) {
            return crow::response(400, "Missing 'from', 'to', or 'date' params");
        }

        // Run the K-Shortest Path Algorithm
        json routes = db.find_smart_routes(src, dst, date, 5); // Get top 5

        return crow::response(routes.dump());
    });

    
    // ==========================================
    // ADMIN ROUTES
    // ==========================================

    CROW_ROUTE(app, "/admin/airport/add").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return crow::response(400, "Invalid JSON");

        Airport apt;
        try { apt = body.get<Airport>(); } catch (...) { return crow::response(400, "Missing fields"); }

        if (db.add_airport(apt)) return crow::response(201, "Airport Added");
        return crow::response(409, "Airport code already exists");
    });

    CROW_ROUTE(app, "/admin/airport/delete").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return crow::response(400, "Invalid JSON");
        
        std::string code = body.value("code", "");
        if (db.delete_airport(code)) return crow::response(200, "Airport Deleted");
        return crow::response(404, "Airport not found");
    });

    CROW_ROUTE(app, "/admin/airport/update").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        const char* code = req.url_params.get("code");
        if (!code) return crow::response(400, "Missing 'code'");

        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return crow::response(400, "Invalid JSON");

        if (db.update_airport(code, body)) return crow::response(200, "Airport Updated");
        return crow::response(404, "Airport not found");
    });

    CROW_ROUTE(app, "/admin/flight/add").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return crow::response(400, "Invalid JSON");

        Flight fl;
        try { fl = body.get<Flight>(); } catch (...) { return crow::response(400, "Missing fields"); }

        if (db.add_flight(fl)) return crow::response(201, "Flight Added");
        return crow::response(409, "Flight ID already exists");
    });

    CROW_ROUTE(app, "/admin/flight/delete").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return crow::response(400, "Invalid JSON");
        
        std::string id = body.value("id", "");
        if (db.delete_flight(id)) return crow::response(200, "Flight Deleted");
        return crow::response(404, "Flight not found");
    });

    CROW_ROUTE(app, "/admin/flight/update").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        const char* id = req.url_params.get("id");
        if (!id) return crow::response(400, "Missing 'id'");

        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return crow::response(400, "Invalid JSON");

        if (db.update_flight(id, body)) return crow::response(200, "Flight Updated");
        return crow::response(404, "Flight not found");
    });

    // Handle 404 with JSON (Good practice for APIs)
    app.catchall_route()
    ([](const crow::request& req, crow::response& res) {
        if (req.method == crow::HTTPMethod::OPTIONS) {
            res.code = 200;
            res.end();
        } else {
            res.code = 404;
            res.body = "{\"error\": \"Route not found\"}";
            res.end();
        }
    });

    int port = 8080;  // Default to 8080 for cloud deployments
    if (const char* env_p = std::getenv("PORT")) {
        try {
            port = std::stoi(env_p);
        } catch (...) {
            std::cerr << "Invalid PORT value, using default 8080" << std::endl;
        }
    }
    
    std::cout << "Server starting on 0.0.0.0:" << port << std::endl;
    
    app.port(port).multithreaded().run();
}