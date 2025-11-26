#include "crow.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

int main()
{
    crow::SimpleApp app;

    // ==========================================
    // ROUTE 1: GET /health
    // Check if server is alive
    // ==========================================
    CROW_ROUTE(app, "/health")
    ([](){
        json response;
        response["status"] = "running";
        response["port"] = 18080;
        return crow::response(response.dump());
    });

    // ==========================================
    // ROUTE 2: POST /api/user
    // Receives JSON data from Frontend
    // ==========================================
    CROW_ROUTE(app, "/api/user").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        
        auto req_body = json::parse(req.body, nullptr, false);

        if (req_body.is_discarded()) {
            return crow::response(400, "Invalid JSON");
        }

        std::string name = req_body["username"];
        int age = req_body["age"];

        json response_data; // json object for response
        response_data["status"] = "success";
        response_data["message"] = "User " + name + " created!";
        response_data["id"] = 101; 

        return crow::response(201, response_data.dump());
    });

    // ==========================================
    // START SERVER
    // ==========================================
    app.port(18080).multithreaded().run();
}