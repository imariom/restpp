/**
 * Copyright (C) 2024 Mário A. Moiane (connect at imariom dot com)
 * Licensed under the MIT license. See LICENSE.txt in the project root for details.
 *
 * Options passed to the HTTP request being made.
 * 
 * For more information and updates, visit: https://github.com/imariom/restpp
 */

#pragma once
#ifndef RESTPP_REQUEST_OPTIONS_HPP
#define RESTPP_REQUEST_OPTIONS_HPP

#include <string>
#include <map>

#include <restpp/core/http_headers.hpp>
#include <restpp/core/details/basic_types.hpp>
#include <restpp/core/http_msg.hpp>
#include <restpp/core/version.hpp>

namespace restpp
{
namespace details
{
    
} // namespace details
    
class abort_signal
{
public:
    void abort();
    bool aborted();
    utility::string_t reason();
};

class request_options
{
public:
    request_options()
    {
        _headers.add("User-Agent", U("restpp/") + utility::string_t{U(RESTPP_VERSION)});
    }

private:
    method _method = "GET";
    http_headers _headers;
    std::string body;
    int timeout = 3000;
    abort_signal _signal;
};
} // namespace restpp

#endif // RESTPP_REQUEST_OPTIONS_HPP