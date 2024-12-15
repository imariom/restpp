#include <restpp/server.hpp>
#include <restpp/http_request.hpp>
#include <restpp/http_response.hpp>
#include <restpp/middleware.hpp>
#include <json.hpp> // For JSON serialization (use nlohmann/json)

int main() {
    using namespace restpp;
    using json = nlohmann::json;



    // advanced route matching
    // middleware support
    // content negotiation
    // support for JSON serialization/deserialization

    // When creating an HTTP request
    http_request request;
    http_request request{"http://localhost:8080"};
    http_request request{"http://localhost:8080", "POST"};
    http_request request{"http://localhost:8080", methods::GET};
    http_request request{uri{"<url>"}, "POST"};
    http_request request{uri{"<url>"}, methods::GET};

    request.method();
    request.set_method("POST");
    request.set_method(methods::);


    // When receiving an HTTP request
    app.get("/search/{id}/{name}", [](const http_request& req) -> http_response {
        // must have
        req.method() // +
        req.path() // +
        req.headers() // +

        auto query = req.query(); // Query parameters 
        auto term = query.at("q");


        auto id = req.params().at("id"); // Path parameters with validation
        auto name = req.params().at("name");
        if (!std::all_of(id.begin(), id.end(), ::isdigit)) {
            return response::bad_request("Invalid user ID");
        }

        req.body() // can implicitly be converted to a string or an input stream
    });


    

    // Create the server
    server app;

    // Middleware: Log all http_requests
    app.use([](const http_request& req, auto next) -> http_response {
        std::cout << "http_request: " << req.method() << " " << req.path() << std::endl;
        return next(req);
    });

    // Middleware: Authorization
    app.use([](const http_request& req, auto next) -> http_response {
        if (req.headers().count("Authorization") == 0) {
            return http_response::unauthorized("Missing Authorization header");
        }
        return next(req);
    });

    // Advanced route: Query parameters
    app.get("/search", [](const http_request& req) -> http_response {
        auto query = req.query();
        auto term = query.at("q");
        return http_response::ok("Searching for: " + term);
    });

    // Advanced route: JSON body parsing
    app.post("/api/data", [](const http_request& req) -> http_response {
        auto body = json::parse(req.body());
        std::string name = body["name"];
        int age = body["age"];
        return http_response::ok("Received name: " + name + ", age: " + std::to_string(age));
    });

    // Advanced route: Path parameters with validation
    app.get("/users/{id}", [](const http_request& req) -> http_response {
        auto id = req.params().at("id");
        if (!std::all_of(id.begin(), id.end(), ::isdigit)) {
            return http_response::bad_http_request("Invalid user ID");
        }
        return http_response::ok("User ID is valid: " + id);
    });

    // Static file serving
    app.serve_static("/static", "./public");

    // Run the server
    app.run(8080);

    return 0;
}
