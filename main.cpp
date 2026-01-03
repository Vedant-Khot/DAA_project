#include "crow.h"
#include "jsondb.h"
#include "Models.h"
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ==========================================
// CORS MIDDLEWARE
// ==========================================
struct CORSHandler {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        // Handle OPTIONS preflight
        if (req.method == crow::HTTPMethod::OPTIONS) {
            std::string origin = req.get_header_value("Origin");
            // Allow Vercel and Localhost
            if (origin.find("vercel.app") != std::string::npos || 
                origin.find("localhost") != std::string::npos || 
                origin.find("127.0.0.1") != std::string::npos) {
                 res.add_header("Access-Control-Allow-Origin", origin);
            } else {
                 // Fallback to the known production frontend
                 res.add_header("Access-Control-Allow-Origin", "https://daa-project-nncj.vercel.app");
            }
            res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
            res.add_header("Access-Control-Allow-Credentials", "true");
            res.code = 200;
            res.end();
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        std::string origin = req.get_header_value("Origin");
        // Allow Vercel and Localhost for main responses too
        if (origin.find("vercel.app") != std::string::npos || 
            origin.find("localhost") != std::string::npos || 
            origin.find("127.0.0.1") != std::string::npos) {
             res.add_header("Access-Control-Allow-Origin", origin);
        } else {
             res.add_header("Access-Control-Allow-Origin", "https://daa-project-nncj.vercel.app");
        }
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
        res.add_header("Access-Control-Allow-Credentials", "true");
    }
};

JsonDB db("flight_database.json");

int main() {
    crow::App<CORSHandler> app;

    // ==========================================
    // 1. PUBLIC ROUTES
    // ==========================================
    
    // Root endpoint with API documentation
    CROW_ROUTE(app, "/")
    ([](){
        json response = {
            {"status", "running"},
            {"api", "Flight Booking API"},
            {"version", "1.0"},
            {"endpoints", {
                {"/health", "Health check"},
                {"/api/airports", "Get all airports"},
                {"/api/flights", "Get flights (limit parameter)"},
                {"/api/search", "Search flights (from, to, date parameters)"}
            }},
            {"booking", {
                {"/api/booking/create", "POST - Create booking with payment"},
                {"/api/booking/get", "GET - Get booking by ID"},
                {"/api/bookings", "GET - Get all bookings"},
                {"/api/booking/user", "GET - Get bookings by email"},
                {"/api/booking/cancel", "POST - Cancel booking"},
                {"/api/admin/stats", "GET - Get real business stats"}
            }},
            {"admin", {
                {"/admin/airport/add", "POST - Add airport"},
                {"/admin/airport/delete", "POST - Delete airport"},
                {"/admin/flight/add", "POST - Add flight"},
                {"/admin/flight/delete", "POST - Delete flight"},
                {"/admin/flight/update", "POST - Update flight"}
            }}
        };
        return crow::response(response.dump());
    });

    // Health check endpoint
    CROW_ROUTE(app, "/health")
    ([](){
        return crow::response("OK");
    });
    
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

    CROW_ROUTE(app, "/api/search-bellman")
    ([](const crow::request& req){
        const char* src = req.url_params.get("from");
        const char* dst = req.url_params.get("to");
        std::string date = "2025-12-01";
        if (req.url_params.get("date")) date = req.url_params.get("date");

        if (!src || !dst) return crow::response(400, "Missing parameters");
        
        return crow::response(db.find_bellman_route(src, dst, date).dump());
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

    // ==========================================
    // 3. BOOKING & PAYMENT ROUTES
    // ==========================================

    // CREATE BOOKING (with simulated payment)
    CROW_ROUTE(app, "/api/booking/create").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)
    ([](const crow::request& req){
        if (req.method == crow::HTTPMethod::OPTIONS) return crow::response(200);

        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return crow::response(400, "Invalid JSON");

        try {
            // Generate unique booking ID
            std::string booking_id = "BK" + std::to_string(std::time(nullptr)) + std::to_string(rand() % 1000);
            
            // Get current date/time for booking timestamp
            std::time_t now = std::time(nullptr);
            char timestamp[20];
            std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M", std::localtime(&now));

            Booking booking;
            booking.booking_id = booking_id;
            booking.user_id = body.value("user_id", "");  // Unique user identifier
            booking.flight_id = body.value("flight_id", "");
            booking.passenger_name = body.value("passenger_name", "");
            booking.passenger_email = body.value("passenger_email", "");
            booking.from_code = body.value("from_code", "");
            booking.to_code = body.value("to_code", "");
            booking.date = body.value("date", "");
            booking.total_price = body.value("total_price", 0);
            booking.booking_date = timestamp;
            booking.status = "confirmed";  // Payment simulated as successful

            if (db.add_booking(booking)) {
                json response = {
                    {"success", true},
                    {"message", "Booking confirmed! Payment successful."},
                    {"booking_id", booking_id},
                    {"status", "confirmed"},
                    {"booking_date", timestamp}
                };
                return crow::response(201, response.dump());
            }
            return crow::response(500, "Failed to create booking");
        } catch (...) { 
            return crow::response(400, "Bad Request"); 
        }
    });

    // GET BOOKING BY ID
    CROW_ROUTE(app, "/api/booking/get")
    ([](const crow::request& req){
        const char* id = req.url_params.get("id");
        if (!id) return crow::response(400, "Missing id parameter");
        
        json booking = db.get_booking_by_id(id);
        if (booking.empty()) return crow::response(404, "Booking not found");
        return crow::response(booking.dump());
    });

    // GET ALL BOOKINGS (Admin)
    CROW_ROUTE(app, "/api/bookings")
    ([](){
        return crow::response(db.get_all_bookings().dump());
    });

    // GET BOOKINGS BY EMAIL
    CROW_ROUTE(app, "/api/booking/user")
    ([](const crow::request& req){
        const char* email = req.url_params.get("email");
        if (!email) return crow::response(400, "Missing email parameter");
        return crow::response(db.get_bookings_by_email(email).dump());
    });

    // GET BOOKINGS BY USER ID (Recommended - more reliable)
    CROW_ROUTE(app, "/api/booking/history")
    ([](const crow::request& req){
        const char* user_id = req.url_params.get("user_id");
        if (!user_id) return crow::response(400, "Missing user_id parameter");
        return crow::response(db.get_bookings_by_user_id(user_id).dump());
    });

    // CANCEL BOOKING
    CROW_ROUTE(app, "/api/booking/cancel").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)
    ([](const crow::request& req){
        if (req.method == crow::HTTPMethod::OPTIONS) return crow::response(200);

        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return crow::response(400, "Invalid JSON");

        std::string booking_id = body.value("booking_id", "");
        if (booking_id.empty()) return crow::response(400, "Missing booking_id");

        if (db.cancel_booking(booking_id)) {
            json response = {
                {"success", true},
                {"message", "Booking cancelled successfully"},
                {"booking_id", booking_id},
                {"status", "cancelled"}
            };
            return crow::response(200, response.dump());
        }
        return crow::response(404, "Booking not found");
    });

    // ADMIN STATS
    CROW_ROUTE(app, "/api/admin/stats")
    ([&](){
        return crow::response(db.get_admin_stats().dump());
    });

    // ==========================================
    // 3. USER MANAGEMENT ROUTES
    // ==========================================
    
    CROW_ROUTE(app, "/api/user/signup")
    .methods("POST"_method)
    ([&](const crow::request& req){
        auto x = json::parse(req.body);
        User u;
        u.id = x.value("id", std::to_string(std::time(nullptr)));
        u.name = x.value("name", "");
        u.email = x.value("email", "");
        u.password = x.value("password", "");
        u.created_at = x.value("created_at", "");
        
        if (db.add_user(u)) {
            return crow::response(200, json({{"success", true}, {"message", "User created"}}).dump());
        }
        return crow::response(400, json({{"success", false}, {"message", "Email already exists"}}).dump());
    });

    CROW_ROUTE(app, "/api/user/login")
    .methods("POST"_method)
    ([&](const crow::request& req){
        auto x = json::parse(req.body);
        std::string email = x.value("email", "");
        std::string password = x.value("password", "");
        
        json u = db.get_user_by_email(email);
        if (!u.empty() && u.value("password", "") == password) {
            return crow::response(200, json({{"success", true}, {"user", u}}).dump());
        }
        return crow::response(401, json({{"success", false}, {"message", "Invalid credentials"}}).dump());
    });

    CROW_ROUTE(app, "/api/users")
    ([&](){
        return crow::response(db.get_all_users().dump());
    });

    // CATCH-ALL
    app.catchall_route()
    ([](const crow::request& req, crow::response& res) {
        res.code = 404;
        res.body = "{\"error\": \"Route not found\"}";
        res.end();
    });

    // ==========================================
    // START SERVER
    // ==========================================
    int port = 18080;  // Default to 8080 for cloud deployment
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