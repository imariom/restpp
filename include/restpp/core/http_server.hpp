/**
 * Copyright (C) 2024 Mário A. Moiane (connect at imariom dot com)
 * Licensed under the MIT license. See LICENSE.txt in the project root for details.
 *
 * HTTP Library: Request and reply message definitions.
 * 
 * For more information and updates, visit: https://github.com/imariom/restpp
 */

#pragma once
#ifndef RESTPP_HTTP_SERVER_HPP
#define RESTPP_HTTP_SERVER_HPP

#include <functional>
#include <map>
#include <string>
#include <boost/asio.hpp>

#include <restpp/core/http_request.hpp>
#include <restpp/core/http_response.hpp>

namespace restpp
{
namespace details
{
/// <summary>
/// Internal representation of an HTTP server.
/// </summary>
class _http_server
{

};
} // namespace details

class http_server
{
public:
    using handler_t = std::function<http_response(const http_request&)>;

    void get(const std::string& route, handler_t handler)
    {
        routes_["GET"][route] = std::move(handler);
    }

    void post(const std::string& route, handler_t handler)
    {
        routes_["POST"][route] = std::move(handler);
    }

    void put(const std::string& route, handler_t handler)
    {
        routes_["PUT"][route] = std::move(handler);
    }

    void delete_(const std::string& route, handler_t handler)
    {
        routes_["DELETE"][route] = std::move(handler);
    }

    void run(unsigned short port)
    {
        // Boost.Asio server logic goes here
        boost::asio::io_context context;
        // Implementation of HTTP server listening on `port`
    }

private:
    std::map<std::string, std::map<std::string, handler_t>> routes_;
};
} // namespace restpp

#endif // RESTPP_HTTP_SERVER_HPP