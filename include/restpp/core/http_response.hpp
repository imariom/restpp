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

    _ASYNCRTIMP utility::string_t to_string() const
    {
        utility::string_t result(_RESTPPSTR("HTTP/1.1 "));
        result += utility::conversions::details::to_string_t(m_status_code);
        result += ' ';
        // If the user didn't explicitly set a reason phrase then we should have it default
        // if they used one of the standard known status codes.
        if (m_reason_phrase.empty())
        {
            result += get_default_reason_phrase(status_code());
        }
        else
        {
            result += m_reason_phrase;
        }

        result += _RESTPPSTR("\r\n");
        result += http_msg_base::to_string();
        return result;
    }

    _http_server_context* _get_server_context() const { return _server_context.get(); }

    void _set_server_context(std::unique_ptr<details::_http_server_context> server_context)
    {
        _server_context = std::move(server_context);
    }

private:
    std::unique_ptr<_http_server_context> _server_context;

    status_code _status_code;
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
    /// Creates a 200 OK response with an optional body.
    /// </summary>
    /// <param name="body">The optional body of the response.</param>
    /// <returns>An HTTP response with status 200 OK.</returns>
    static http_response ok(const utility::string_t& body = U(""))
    {
        return http_response(status_codes::OK, body);
    }

    /// <summary>
    /// Creates a 201 Created response with an optional body.
    /// </summary>
    /// <param name="body">The optional body of the response.</param>
    /// <returns>An HTTP response with status 201 Created.</returns>
    static http_response created(const utility::string_t& body = U(""))
    {
        return http_response(status_codes::Created, body);
    }

    /// <summary>
    /// Creates a 204 No Content response.
    /// </summary>
    /// <returns>An HTTP response with status 204 No Content.</returns>
    static http_response no_content()
    {
        return http_response(status_codes::NoContent);
    }

    /// <summary>
    /// Creates a 400 Bad Request response with an optional body.
    /// </summary>
    /// <param name="body">The optional body of the response.</param>
    /// <returns>An HTTP response with status 400 Bad Request.</returns>
    static http_response bad_request(const utility::string_t& body = U(""))
    {
        return http_response(status_codes::BadRequest, body);
    }

    /// <summary>
    /// Creates a 401 Unauthorized response with an optional body.
    /// </summary>
    /// <param name="body">The optional body of the response.</param>
    /// <returns>An HTTP response with status 401 Unauthorized.</returns>
    static http_response unauthorized(const utility::string_t& body = U(""))
    {
        return http_response(status_codes::Unauthorized, body);
    }

    /// <summary>
    /// Creates a 403 Forbidden response with an optional body.
    /// </summary>
    /// <param name="body">The optional body of the response.</param>
    /// <returns>An HTTP response with status 403 Forbidden.</returns>
    static http_response forbidden(const utility::string_t& body = U(""))
    {
        return http_response(status_codes::Forbidden, body);
    }

    /// <summary>
    /// Creates a 404 Not Found response with an optional body.
    /// </summary>
    /// <param name="body">The optional body of the response.</param>
    /// <returns>An HTTP response with status 404 Not Found.</returns>
    static http_response not_found(const utility::string_t& body = U(""))
    {
        return http_response(status_codes::NotFound, body);
    }

    /// <summary>
    /// Creates a 405 Method Not Allowed response with an optional body.
    /// </summary>
    /// <param name="body">The optional body of the response.</param>
    /// <returns>An HTTP response with status 405 Method Not Allowed.</returns>
    static http_response method_not_allowed(const utility::string_t& body = U(""))
    {
        return http_response(status_codes::MethodNotAllowed, body);
    }

    /// <summary>
    /// Creates a 409 Conflict response with an optional body.
    /// </summary>
    /// <param name="body">The optional body of the response.</param>
    /// <returns>An HTTP response with status 409 Conflict.</returns>
    static http_response conflict(const utility::string_t& body = U(""))
    {
        return http_response(status_codes::Conflict, body);
    }

    /// <summary>
    /// Creates a 500 Internal Server Error response with an optional body.
    /// </summary>
    /// <param name="body">The optional body of the response.</param>
    /// <returns>An HTTP response with status 500 Internal Server Error.</returns>
    static http_response internal_server_error(const utility::string_t& body = U(""))
    {
        return http_response(status_codes::InternalError, body);
    }

    /// <summary>
    /// Creates a 501 Not Implemented response with an optional body.
    /// </summary>
    /// <param name="body">The optional body of the response.</param>
    /// <returns>An HTTP response with status 501 Not Implemented.</returns>
    static http_response not_implemented(const utility::string_t& body = U(""))
    {
        return http_response(status_codes::NotImplemented, body);
    }

    /// <summary>
    /// Creates a 503 Service Unavailable response with an optional body.
    /// </summary>
    /// <param name="body">The optional body of the response.</param>
    /// <returns>An HTTP response with status 503 Service Unavailable.</returns>
    static http_response service_unavailable(const utility::string_t& body = U(""))
    {
        return http_response(status_codes::ServiceUnavailable, body);
    }

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
    http::details::_http_server_context* _get_server_context() const { return _impl->_get_server_context(); }
    void _set_server_context(std::unique_ptr<http::details::_http_server_context> server_context)
    {
        _impl->_set_server_context(std::move(server_context));
    }

private:
   std::shared_ptr<details::_http_response> _impl;
};
} // namespace restpp

#endif // RESTPP_HTTP_RESPONSE_HPP