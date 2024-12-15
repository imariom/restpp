#include <iostream>

#include <restpp/restpp.hpp>

int main()
{
    restpp::fetch();
    restpp::server app;
    restpp::http_request request;
    restpp::http_response response;
    restpp::fetch_options options;
    
    auto res0 = restpp::fetch("http://www.google.com");
    std::cout << "Status code: " << res0.status_code << '\n';
    std::cout << "Headers: "     << res0.headers << '\n';
    std::cout << "Body: "        << res0.body << '\n';

    // Fetch a remote resource
    // -----------------------
    // fetch(rstpp::request)
    // fetch(const char*)
    // fetch(const char*, options)
    // fetch(std::string)
    // fetch(std::string, options)
    // fetch(Tchar)
    // fetch(Tchar, options)

    // Fetch a local resource
    // ----------------------
    // fetch(rstpp::request)
    // fetch(const char*)
    // fetch(const char*, options)
    // fetch(std::string)
    // fetch(std::string, options)
    // fetch(Tchar)
    // fetch(Tchar, options)

    return 0;
}

