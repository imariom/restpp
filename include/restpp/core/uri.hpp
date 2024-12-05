/***
 * Copyright (C) 2024-present Mário A. Moiane (connect at imariom dot com)
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * Protocol independent support for URIs.
 *
 * For the latest on this and related APIs, please see: https://github.com/imariom/restpp
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 ****/


#ifndef RESTPP_URI_H
#define RESTPP_URI_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <exception>
#include <cstring>

#include <restpp/core/asyncrt_utils.hpp>
#include <restpp/core/details/basic_types.hpp>

namespace restpp
{
namespace details
{
struct uri_components
{
    uri_components() : _path(_RESTPPSTR("/")), _port(-1) {}

    uri_components(const uri_components&) = default;
    uri_components& operator=(const uri_components&) = default;

    uri_components(uri_components&& other) noexcept : _scheme(std::move(other._scheme)),
                                                      _host(std::move(other._host)),
                                                      _userInfo(std::move(other._userInfo)),
                                                      _path(std::move(other._path)),
                                                      _query(std::move(other._query)),
                                                      _fragment(std::move(other._fragment)),
                                                      _port(std::move(other._port))
    {
    }

    uri_components& operator=(uri_components&& other) noexcept
    {
        if (this != &other)
        {
            _scheme = std::move(other._scheme);
            _host = std::move(other._host);
            _userInfo = std::move(other._userInfo);
            _path = std::move(other._path);
            _query = std::move(other._query);
            _fragment = std::move(other._fragment);
            _port = other._port;
        }
        return *this;
    }

    utility::string_t join();

    utility::string_t _scheme;
    utility::string_t _host;
    utility::string_t _userInfo;
    utility::string_t _path;
    utility::string_t _query;
    utility::string_t _fragment;
    int _port;
};

namespace 
{
/// <summary>
/// Unreserved characters are those that are allowed in a URI but do not have a reserved purpose. They include:
/// - A-Z
/// - a-z
/// - 0-9
/// - '-' (hyphen)
/// - '.' (period)
/// - '_' (underscore)
/// - '~' (tilde)
/// </summary>
inline bool is_unreserved(int c)
{
    return ::utility::details::is_alnum((char)c) || c == '-' || c == '.' || c == '_' || c == '~';
}

/// <summary>
/// General delimiters serve as the delimiters between different uri components.
/// General delimiters include:
/// - All of these :/?#[]@
/// </summary>
inline bool is_gen_delim(int c)
{
    return c == ':' || c == '/' || c == '?' || c == '#' || c == '[' || c == ']' || c == '@';
}

/// <summary>
/// Subdelimiters are those characters that may have a defined meaning within component
/// of a uri for a particular scheme. They do not serve as delimiters in any case between
/// uri segments. sub_delimiters include:
/// - All of these !$&'()*+,;=
/// </summary>
inline bool is_sub_delim(int c)
{
    switch (c)
    {
        case '!':
        case '$':
        case '&':
        case '\'':
        case '(':
        case ')':
        case '*':
        case '+':
        case ',':
        case ';':
        case '=': return true;
        default: return false;
    }
}

/// <summary>
/// Reserved characters includes the general delimiters and sub delimiters. Some characters
/// are neither reserved nor unreserved, and must be percent-encoded.
/// </summary>
inline bool is_reserved(int c) { return is_gen_delim(c) || is_sub_delim(c); }

/// <summary>
/// Legal characters in the scheme portion include:
/// - Any alphanumeric character
/// - '+' (plus)
/// - '-' (hyphen)
/// - '.' (period)
///
/// Note that the scheme must BEGIN with an alpha character.
/// </summary>
inline bool is_scheme_character(int c)
{
    return ::utility::details::is_alnum((char)c) || c == '+' || c == '-' || c == '.';
}

/// <summary>
/// Legal characters in the user information portion include:
/// - Any unreserved character
/// - The percent character ('%'), and thus any percent-endcoded octet
/// - The sub-delimiters
/// - ':' (colon)
/// </summary>
inline bool is_user_info_character(int c) { return is_unreserved(c) || is_sub_delim(c) || c == '%' || c == ':'; }

/// <summary>
/// Legal characters in the authority portion include:
/// - Any unreserved character
/// - The percent character ('%'), and thus any percent-endcoded octet
/// - The sub-delimiters
/// - ':' (colon)
/// - IPv6 requires '[]' allowed for it to be valid URI and passed to underlying platform for IPv6 support
/// </summary>
inline bool is_authority_character(int c)
{
    return is_unreserved(c) || is_sub_delim(c) || c == '%' || c == '@' || c == ':' || c == '[' || c == ']';
}

/// <summary>
/// Legal characters in the path portion include:
/// - Any unreserved character
/// - The percent character ('%'), and thus any percent-endcoded octet
/// - The sub-delimiters
/// - ':' (colon)
/// - '@' (at sign)
/// </summary>
inline bool is_path_character(int c)
{
    return is_unreserved(c) || is_sub_delim(c) || c == '%' || c == '/' || c == ':' || c == '@';
}

/// <summary>
/// Legal characters in the query portion include:
/// - Any path character
/// - '?' (question mark)
/// </summary>
inline bool is_query_character(int c) { return is_path_character(c) || c == '?'; }

/// <summary>
/// Legal characters in the fragment portion include:
/// - Any path character
/// - '?' (question mark)
/// </summary>
inline bool is_fragment_character(int c)
{
    // this is intentional, they have the same set of legal characters
    return is_query_character(c);
}

struct inner_parse_out
{
    const utility::char_t* scheme_begin = nullptr;
    const utility::char_t* scheme_end = nullptr;
    const utility::char_t* uinfo_begin = nullptr;
    const utility::char_t* uinfo_end = nullptr;
    const utility::char_t* host_begin = nullptr;
    const utility::char_t* host_end = nullptr;
    int port = 0;
    const utility::char_t* path_begin = nullptr;
    const utility::char_t* path_end = nullptr;
    const utility::char_t* query_begin = nullptr;
    const utility::char_t* query_end = nullptr;
    const utility::char_t* fragment_begin = nullptr;
    const utility::char_t* fragment_end = nullptr;

    /// <summary>
    /// Parses the uri, setting the given pointers to locations inside the given buffer.
    /// 'encoded' is expected to point to an encoded zero-terminated string containing a uri
    /// </summary>
    bool parse_from(const utility::char_t* encoded)
    {
        const utility::char_t* p = encoded;

        // IMPORTANT -- A uri may either be an absolute uri, or an relative-reference
        // Absolute: 'http://host.com'
        // Relative-Reference: '//:host.com', '/path1/path2?query', './path1:path2'
        // A Relative-Reference can be disambiguated by parsing for a ':' before the first slash

        bool is_relative_reference = true;
        const utility::char_t* p2 = p;
        for (; *p2 != _RESTPPSTR('/') && *p2 != _RESTPPSTR('\0'); p2++)
        {
            if (*p2 == _RESTPPSTR(':'))
            {
                // found a colon, the first portion is a scheme
                is_relative_reference = false;
                break;
            }
        }

        if (!is_relative_reference)
        {
            // the first character of a scheme must be a letter
            if (!isalpha(*p))
            {
                return false;
            }

            // start parsing the scheme, it's always delimited by a colon (must be present)
            scheme_begin = p++;
            for (; *p != ':'; p++)
            {
                if (!is_scheme_character(*p))
                {
                    return false;
                }
            }
            scheme_end = p;

            // skip over the colon
            p++;
        }

        // if we see two slashes next, then we're going to parse the authority portion
        // later on we'll break up the authority into the port and host
        const utility::char_t* authority_begin = nullptr;
        const utility::char_t* authority_end = nullptr;
        if (*p == _RESTPPSTR('/') && p[1] == _RESTPPSTR('/'))
        {
            // skip over the slashes
            p += 2;
            authority_begin = p;

            // the authority is delimited by a slash (resource), question-mark (query) or octothorpe (fragment)
            // or by EOS. The authority could be empty ('file:///C:\file_name.txt')
            for (; *p != _RESTPPSTR('/') && *p != _RESTPPSTR('?') && *p != _RESTPPSTR('#') && *p != _RESTPPSTR('\0'); p++)
            {
                // We're NOT currently supporting IPvFuture or username/password in authority
                // IPv6 as the host (i.e. http://[:::::::]) is allowed as valid URI and passed to subsystem for support.
                if (!is_authority_character(*p))
                {
                    return false;
                }
            }
            authority_end = p;

            // now lets see if we have a port specified -- by working back from the end
            if (authority_begin != authority_end)
            {
                // the port is made up of all digits
                const utility::char_t* port_begin = authority_end - 1;
                for (; isdigit(*port_begin) && port_begin != authority_begin; port_begin--)
                {
                }

                if (*port_begin == _RESTPPSTR(':'))
                {
                    // has a port
                    host_begin = authority_begin;
                    host_end = port_begin;

                    // skip the colon
                    port_begin++;

                    port =
                        utility::conversions::details::scan_string<int>(utility::string_t(port_begin, authority_end));
                }
                else
                {
                    // no port
                    host_begin = authority_begin;
                    host_end = authority_end;
                }

                // look for a user_info component
                const utility::char_t* u_end = host_begin;
                for (; is_user_info_character(*u_end) && u_end != host_end; u_end++)
                {
                }

                if (*u_end == _RESTPPSTR('@'))
                {
                    host_begin = u_end + 1;
                    uinfo_begin = authority_begin;
                    uinfo_end = u_end;
                }
            }
        }

        // if we see a path character or a slash, then the
        // if we see a slash, or any other legal path character, parse the path next
        if (*p == _RESTPPSTR('/') || is_path_character(*p))
        {
            path_begin = p;

            // the path is delimited by a question-mark (query) or octothorpe (fragment) or by EOS
            for (; *p != _RESTPPSTR('?') && *p != _RESTPPSTR('#') && *p != _RESTPPSTR('\0'); p++)
            {
                if (!is_path_character(*p))
                {
                    return false;
                }
            }
            path_end = p;
        }

        // if we see a ?, then the query is next
        if (*p == _RESTPPSTR('?'))
        {
            // skip over the question mark
            p++;
            query_begin = p;

            // the query is delimited by a '#' (fragment) or EOS
            for (; *p != _RESTPPSTR('#') && *p != _RESTPPSTR('\0'); p++)
            {
                if (!is_query_character(*p))
                {
                    return false;
                }
            }
            query_end = p;
        }

        // if we see a #, then the fragment is next
        if (*p == _RESTPPSTR('#'))
        {
            // skip over the hash mark
            p++;
            fragment_begin = p;

            // the fragment is delimited by EOS
            for (; *p != _RESTPPSTR('\0'); p++)
            {
                if (!is_fragment_character(*p))
                {
                    return false;
                }
            }
            fragment_end = p;
        }

        return true;
    }

    void write_to(uri_components& components)
    {
        if (scheme_begin)
        {
            components._scheme.assign(scheme_begin, scheme_end);
            utility::details::inplace_tolower(components._scheme);
        }
        else
        {
            components._scheme.clear();
        }

        if (uinfo_begin)
        {
            components._userInfo.assign(uinfo_begin, uinfo_end);
        }

        if (host_begin)
        {
            components._host.assign(host_begin, host_end);
            utility::details::inplace_tolower(components._host);
        }
        else
        {
            components._host.clear();
        }

        components._port = port;

        if (path_begin)
        {
            components._path.assign(path_begin, path_end);
        }
        else
        {
            // default path to begin with a slash for easy comparison
            components._path = _RESTPPSTR("/");
        }

        if (query_begin)
        {
            components._query.assign(query_begin, query_end);
        }
        else
        {
            components._query.clear();
        }

        if (fragment_begin)
        {
            components._fragment.assign(fragment_begin, fragment_end);
        }
        else
        {
            components._fragment.clear();
        }
    }
};
} // namespace

} // namespace details

/// <summary>
/// A single exception type to represent errors in parsing, encoding, and decoding URIs.
/// </summary>
class uri_exception : public std::exception
{
public:
    uri_exception(utility::string_t msg) : _msg{std::move(msg)} {}

    ~uri_exception() noexcept {}

    const utility::char_t* what() noexcept { return _msg.c_str(); }

private:
    utility::string_t _msg;
};

class uri
{
public:
    /// <summary>
    /// Creates a URI from the given encoded string. This will throw an exception if the string
    /// does not contain a valid URI. Use uri::validate if processing user-input.
    /// </summary>
    /// <param name="uri_string">A pointer to an encoded string to create the URI instance.</param>
    uri(const utility::char_t* uri_string)
    {
        if (std::strlen(uri_string) == 0)
            throw uri_exception{"provided uri is invalid: " + std::string{uri_string}};
        _url = uri_string;
        parse();
    }

    // <summary>
    /// Creates a URI from the given encoded string. This will throw an exception if the string
    /// does not contain a valid URI. Use uri::validate if processing user-input.
    /// </summary>
    /// <param name="uri_string">An encoded URI string to create the URI instance.</param>
    uri(const utility::string_t& uri_string)
        : uri(uri_string.c_str())
    {
    }

    /// <summary>
    /// Copy constructor.
    /// </summary>
    uri(const uri&) = default;

    /// <summary>
    /// Copy assignment operator.
    /// </summary>
    uri& operator=(const uri&) = default;

    /// <summary>
    /// Move constructor.
    /// </summary>
    uri(uri&& other) noexcept : _uri{std::move(other._uri)} {}

    /// <summary>
    /// Move assignment operator
    /// </summary>
    uri& operator=(uri&& other) noexcept
    {
        if (this != &other)
            _uri = std::move(other._uri);
        return *this;
    }

    /// <summary>
    /// Conversion operator from std::string to uri.
    /// </summary>
    uri& operator=(const utility::string_t& url)
    {
        _url = url;
        parse();
        return *this;
    }

    /// <summary>
    /// Conversion operator from const char* to uri.
    /// </summary>
    uri& operator=(const utility::char_t* url)
    {
        _url = url;
        parse();
        return *this;
    }

    /// <summary>
    /// Get the scheme component of the URI as an encoded string.
    /// </summary>
    /// <returns>The URI scheme as a string.</returns>
    const utility::string_t& scheme() const {
        return _components._scheme;
    }

    /// <summary>
    /// Get the user information component of the URI as an encoded string.
    /// </summary>
    /// <returns>The URI user information as a string.</returns>
    const utility::string_t& user_info() const {
        return _components._userInfo;
    }

    /// <summary>
    /// Get the host component of the URI as an encoded string.
    /// </summary>
    /// <returns>The URI host as a string.</returns>
    const utility::string_t& host() {
        return _components._host;
    }

    /// <summary>
    /// Get the port component of the URI. Returns -1 if no port is specified.
    /// </summary>
    /// <returns>The URI port as an integer.</returns>
    int port() const {
        return _components._port;
    }

    /// <summary>
    /// Get the path component of the URI as an encoded string.
    /// </summary>
    /// <returns>The URI path as a string.</returns>
    const utility::string_t& path() {
        return _components._path;
    }

    /// <summary>
    /// Get the query component of the URI as an encoded string.
    /// </summary>
    /// <returns>The URI query as a string.</returns>
    const utility::string_t& query() const {
        return _components._query;
    }

    /// <summary>
    /// Get the fragment component of the URI as an encoded string.
    /// </summary>
    /// <returns>The URI fragment as a string.</returns>
    const utility::string_t& fragment() const {
        return _components._fragment;
    }

private:
    void parse() {
        // very basic URI parsing
        auto scheme_end = _url.find("://");
        _components._scheme = _url.substr(0, scheme_end);

        auto host_start = scheme_end + 3;
        auto path_start = _url.find("/", host_start);
        _components._host = _url.substr(host_start, path_start - host_start);

        auto port_pos = _components._host.find(":");
        if (port_pos != utility::string_t::npos) {
            _components._port = std::atoi(_components._host.substr(port_pos + 1).c_str());
            _components._host = _components._host.substr(0, port_pos);
        } else {
            _components._port = (_components._scheme == "https") ? 443 : 80;
        }

        _components._path = (path_start == utility::string_t::npos) ? "/" : _url.substr(path_start);
    }

private:
    utility::string_t _url;
    utility::string_t _uri;
    details::uri_components _components;
};

} // namespace restpp

#endif // RESTPP_URI_H
