//
// uri.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2024 Mário A. Moiane (connect at imariom dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef RESTPP_URI_H
#define RESTPP_URI_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <exception>

namespace restpp
{

class uri
{
public:
    uri(const std::string& url)
        : _url{url}
    {
        if (url.length() == 0)
            throw std::exception{};
        init();
    }

    uri(const char* url)
        : uri{std::string{url}}
    {
    }

    uri& operator=(const std::string& url)
    {
        _url = url;
        init();
        return *this;
    }

    uri& operator=(const char* url)
    {
        _url = url;
        init();
        return *this;
    }

    std::string host() {
        return _host;
    }

    std::string port() {
        return _port;
    }

    std::string path() {
        return _path;
    }

    std::string scheme() {
        return _scheme;
    }

    std::string url() {
        return _url;
    }

private:
    void init() {
        // very basic URI parsing (extend as needed)
        auto scheme_end = _url.find("://");
        _scheme = _url.substr(0, scheme_end);

        auto host_start = scheme_end + 3;
        auto path_start = _url.find("/", host_start);
        _host = _url.substr(host_start, path_start - host_start);

        auto port_pos = _host.find(":");
        if (port_pos != std::string::npos) {
            _port = _host.substr(port_pos + 1);
            _host = _host.substr(0, port_pos);
        } else {
            _port = (_scheme == "https") ? "443" : "80";
        }

        _path = (path_start == std::string::npos) ? "/" : _url.substr(path_start);
    }

private:
    std::string _url;
    std::string _scheme;
    std::string _host;
    std::string _path;
    std::string _port;
};

} // namespace restpp

#endif // RESTPP_URI_H
