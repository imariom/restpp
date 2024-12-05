/***
 * Copyright (C) 2024-present Mário A. Moiane (connect at imariom dot com)
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * HTTP response from the remote server.
 *
 * For the latest on this and related APIs, please see: https://github.com/imariom/restpp
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 ****/

#pragma once
#ifndef RESTPP_RESPONSE_H
#define RESTPP_RESPONSE_H

#include <string>

namespace restpp
{

struct response
{
    int status_code;
    std::string headers;
    std::string body;
};

} // namespace restpp

#endif // RESTPP_RESPONSE_H
