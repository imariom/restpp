//
// response.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2024 Mário A. Moiane (connect at imariom dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef RESTPP_RESPONSE_H
#define RESTPP_RESPONSE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

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
