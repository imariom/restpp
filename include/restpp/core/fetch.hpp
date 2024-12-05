/***
 * Copyright (C) 2024-present Mário A. Moiane (connect at imariom dot com)
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * Fetch function that will fetches a local or remote resource.
 *
 * For the latest on this and related APIs, please see: https://github.com/imariom/restpp
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 ****/

#pragma once
#ifndef RESTPP_FETCH_HPP
#define RESTPP_FETCH_HPP

#include <iostream>
#include <boost/asio.hpp>

#include <restpp/core/uri.hpp>
#include <restpp/core/options.hpp>
#include <restpp/core/response.hpp>

namespace restpp
{
response fetch(uri _path, options _options = {}) {
    try {
        boost::asio::io_context io_context;

        // Resolve the host and port
        boost::asio::ip::tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(_path.host(), std::to_string(_path.port()));

        // Create the socket
        boost::asio::ip::tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        // Form the HTTP request
        std::ostringstream request_stream;
        request_stream << _options.method << " " << _path.path() << " HTTP/1.1\r\n";
        request_stream << "Host: " << _path.host() << "\r\n";
        request_stream << "Connection: close\r\n";

        // Add custom headers
        for (const auto& [key, value] : _options.headers) {
            request_stream << key << ": " << value << "\r\n";
        }
        request_stream << "\r\n";

        // Send the request
        std::string request = request_stream.str();
        boost::asio::write(socket, boost::asio::buffer(request));

        // Read the response
        boost::asio::streambuf response_buffer;
        boost::asio::read_until(socket, response_buffer, "\r\n");

        // Parse the response status line
        std::istream response_stream(&response_buffer);
        std::string http_version;
        unsigned int status_code;
        std::string status_message;
        response_stream >> http_version >> status_code;
        std::getline(response_stream, status_message);

        if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
            throw std::runtime_error("Invalid response");
        }

        // Read headers
        std::ostringstream headers;
        std::string header_line;
        while (std::getline(response_stream, header_line) && header_line != "\r") {
            headers << header_line << "\n";
        }

        // Read the body
        std::ostringstream body;
        if (response_buffer.size() > 0) {
            body << &response_buffer;
        }

        // Continue reading the body until EOF
        boost::system::error_code error;
        while (boost::asio::read(socket, response_buffer,
                                 boost::asio::transfer_at_least(1), error)) {
            body << &response_buffer;
        }
        if (error != boost::asio::error::eof) {
            throw boost::system::system_error(error);
        }

        // Construct the response object
        return {static_cast<int>(status_code), headers.str(), body.str()};
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return {500, "", e.what()};
    }
}

} // namespace restpp

#endif // RESTPP_FETCH_HPP
