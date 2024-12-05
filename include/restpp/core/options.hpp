/***
 * Copyright (C) 2024-present Mário A. Moiane (connect at imariom dot com)
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * Options passed to the HTTP request being made.
 *
 * For the latest on this and related APIs, please see: https://github.com/imariom/restpp
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 ****/

#pragma once
#ifndef RESTPP_OPTIONS_HPP
#define RESTPP_OPTIONS_HPP

#include <string>
#include <map>

namespace restpp
{

struct options
{
    options()
    {
        method = "GET";
        headers.insert({"User-Agent", "restpp.io client"});
    }

    std::string method;
    std::map<std::string, std::string> headers;
};

} // namespace restpp

#endif // RESTPP_OPTIONS_HPP
