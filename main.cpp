#include "crow.h"
#include "JsonDB.h"
#include <iostream>
#include <string>

// ==========================================
// CORS MIDDLEWARE
// ==========================================
struct CORSHandler {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        // No action needed
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        // Always add these headers to every response
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
    }
};

JsonDB db("flight_database.json");

int main() {
    crow::App<CORSHandler> app;

    // ==========================================
    // 1. PUBLIC ROUTES
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
        std::string date = "2025-12-01";
        if (req.url_params.get("date")) date = req.url_params.get("date");

        if (!src || !dst) return crow::response(400, "Missing parameters");
        
        return crow::response(db.find_smart_routes(src, dst, date, 5).dump());
    });

    // ==========================================
    // 2. ADMIN ROUTES (Fixed for CORS)
    // We add OPTIONS method to all of them
    // ==========================================

    // ADD AIRPORT
    CROW_ROUTE(app, "/admin/airport/add").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)
    ([](const crow::request& req){
        if (req.method == crow::HTTPMethod::OPTIONS) return crow::response(200); // Handle Preflight
        
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return crow::response(400);
        try {
            if (db.add_airport(body.get<Airport>())) return crow::response(201, "Added");
            return crow::response(409, "Exists");
        } catch(...) { return crow::response(400); }
    });

    // DELETE AIRPORT
    CROW_ROUTE(app, "/admin/airport/delete").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)
    ([](const crow::request& req){
        if (req.method == crow::HTTPMethod::OPTIONS) return crow::response(200);

        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return crow::response(400);
        std::string code = body.value("code", "");
        if (db.delete_airport(code)) return crow::response(200, "Deleted");
        return crow::response(404, "Not Found");
    });

    // ADD FLIGHT
    CROW_ROUTE(app, "/admin/flight/add").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)
    ([](const crow::request& req){
        if (req.method == crow::HTTPMethod::OPTIONS) return crow::response(200); // Handle Preflight

        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return crow::response(400, "Invalid JSON");

        try {
            Flight fl = body.get<Flight>();
            if (db.add_flight(fl)) return crow::response(201, "Added");
            return crow::response(409, "Exists");
        } catch (...) { return crow::response(400, "Bad Request"); }
    });

    // DELETE FLIGHT
    CROW_ROUTE(app, "/admin/flight/delete").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)
    ([](const crow::request& req){
        if (req.method == crow::HTTPMethod::OPTIONS) return crow::response(200);

        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return crow::response(400);
        std::string id = body.value("id", "");
        if (db.delete_flight(id)) return crow::response(200, "Deleted");
        return crow::response(404, "Not Found");
    });

    // UPDATE FLIGHT
    CROW_ROUTE(app, "/admin/flight/update").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)
    ([](const crow::request& req){
        if (req.method == crow::HTTPMethod::OPTIONS) return crow::response(200);

        const char* id = req.url_params.get("id");
        if (!id) return crow::response(400, "Missing id");
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return crow::response(400);
        if (db.update_flight(id, body)) return crow::response(200, "Updated");
        return crow::response(404, "Not Found");
    });

    // CATCH-ALL (Backup for other OPTIONS requests)
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

    // ==========================================
    // START SERVER
    // ==========================================
    int port = 18080;
    if (const char* env_p = std::getenv("PORT")) {
        port = std::stoi(env_p);
    }
    
    std::cout << "Server starting on port " << port << "..." << std::endl;
    app.port(port).multithreaded().run();
}