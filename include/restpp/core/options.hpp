//
// options.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2024 Mário A. Moiane (connect at imariom dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef RESTPP_OPTIONS_HPP
#define RESTPP_OPTIONS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

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
