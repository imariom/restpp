/**
 * Copyright (C) 2024 Mário A. Moiane (connect at imariom dot com)
 * Licensed under the MIT license. See LICENSE.txt in the project root for details.
 *
 * Defines a class for representing and handling HTTP responses.
 * 
 * For more information and updates, visit: https://github.com/imariom/restpp
 */

#pragma once
#ifndef RESTPP_HTTP_RESPONSE_HPP
#define RESTPP_HTTP_RESPONSE_HPP

#include <limits>

#include <restpp/core/details/basic_types.hpp>
#include <restpp/core/http_msg.hpp>
#include <restpp/core/http_headers.hpp>

namespace restpp
{
namespace details
{
/// <summary>
/// Internal representation of an HTTP response.
/// </summary>
class _http_response final : public details::http_msg_base
{
public:
    _http_response() : _status_code((std::numeric_limits<uint16_t>::max)()) {}

    _http_response(status_code code) : _status_code(code) {}

    status_code status_code() const { return _status_code; }

    void set_status_code(status_code code) { _status_code = code; }

    const reason_phrase& reason_phrase() const { return _reason_phrase; }

    void set_reason_phrase(const reason_phrase& reason) { _reason_phrase = reason; }

    // TODO: implement this functioinstreamn
    utility::string_t body() { throw std::exception{}; }

    _ASYNCRTIMP utility::string_t to_string() const
    {
        utility::string_t result(_RESTPPSTR("HTTP/1.1 "));
        result += utility::conversions::details::to_string_t(_status_code);
        result += ' ';
        // If the user didn't explicitly set a reason phrase then we should have it default
        // if they used one of the standard known status codes.
        if (_reason_phrase.empty())
        {
            result += get_default_reason_phrase(status_code());
        }
        else
        {
            result += _reason_phrase;
        }

        result += _RESTPPSTR("\r\n");
        result += http_msg_base::to_string();
        return result;
    }

private:
    status_codes _status_code;
    reason_phrase _reason_phrase;
};
} // namespace details

/// <summary>
/// Represents an HTTP response and encapsulates standard HTTP responses.
/// </summary>
class http_response
{
public:
    /// <summary>
    /// Constructs a response with an empty status code, no headers, and no body.
    /// </summary>
    /// <returns>A new HTTP response.</returns>
    http_response() : _impl(std::make_shared<details::_http_response>()) {}

    /// <summary>
    /// Constructs a response with given status code, no headers, and no body.
    /// </summary>
    /// <param name="code">HTTP status code to use in response.</param>
    /// <returns>A new HTTP response.</returns>
    http_response(http::status_code code) : _impl(std::make_shared<details::_http_response>(code)) {}

    /// <summary>
    /// Gets the status code of the response message.
    /// </summary>
    /// <returns>status code.</returns>
    http::status_code status_code() const { return _impl->status_code(); }

    /// <summary>
    /// Sets the status code of the response message.
    /// </summary>
    /// <param name="status_code">Status code to set.</param>
    /// <remarks>
    /// This will overwrite any previously set status code.
    /// </remarks>
    http_response& set_status_code(int status_code) const { _impl->set_status_code(code); }

    /// <summary>
    /// Gets the reason phrase of the response message.
    /// If no reason phrase is set it will default to the standard one corresponding to the status code.
    /// </summary>
    /// <returns>Reason phrase.</returns>
    const http::reason_phrase& reason_phrase() const { return _impl->reason_phrase(); }

    /// <summary>
    /// Sets the reason phrase of the response message.
    /// If no reason phrase is set it will default to the standard one corresponding to the status code.
    /// </summary>
    /// <param name="reason">The reason phrase to set.</param>
    void set_reason_phrase(const http::reason_phrase& reason) const { _impl->set_reason_phrase(reason); }

    /// <summary>
    /// Gets the headers of the response message.
    /// </summary>
    /// <returns>HTTP headers for this response.</returns>
    /// <remarks>
    /// Use the <seealso cref="http_headers::add Method"/> to fill in desired headers.
    /// </remarks>
    http_headers& headers() { return _impl->headers(); }

    /// <summary>
    /// Gets a const reference to the headers of the response message.
    /// </summary>
    /// <returns>HTTP headers for this response.</returns>
    const http_headers& headers() const { return _impl->headers(); }

    /// <summary>
    /// Produces a stream which the caller may use to retrieve data from an incoming request.
    /// </summary>
    /// <returns>A readable, open asynchronous stream.</returns>
    /// <remarks>
    /// This cannot be used in conjunction with any other means of getting the body of the request.
    /// It is not necessary to wait until the message has been sent before starting to write to the
    /// stream, but it is advisable to do so, since it will allow the network I/O to start earlier
    /// and the work of sending data can be overlapped with the production of more data.
    /// </remarks>
    utility::string_t body() const { return _impl->body(); }

    /// <summary>
    /// Generates a string representation of the message, including the body when possible.
    /// Mainly this should be used for debugging purposes as it has to copy the
    /// message body and doesn't have excellent performance.
    /// </summary>
    /// <returns>A string representation of this HTTP request.</returns>
    /// <remarks>Note this function is synchronous and doesn't wait for the
    /// entire message body to arrive. If the message body has arrived by the time this
    /// function is called and it is has a textual Content-Type it will be included.
    /// Otherwise just the headers will be present.</remarks>
    utility::string_t to_string() const { return _impl->to_string(); }

    std::shared_ptr<http::details::_http_response> _get_impl() const { return _impl; }

private:
   std::shared_ptr<details::_http_response> _impl;
};
} // namespace restpp

#endif // RESTPP_HTTP_RESPONSE_HPP
