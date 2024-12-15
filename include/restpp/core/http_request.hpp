/**
 * Copyright (C) 2024 Mário A. Moiane (connect at imariom dot com)
 * Licensed under the MIT license. See LICENSE.txt in the project root for details.
 *
 * Defines a class for constructing and handling HTTP requests.
 * 
 * For more information and updates, visit: https://github.com/imariom/restpp
 */

#pragma once
#ifndef RESTPP_HTTP_REQUEST_HPP
#define RESTPP_HTTP_REQUEST_HPP

#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <vector>

#include <restpp/core/details/basic_types.hpp>
#include <restpp/core/http_msg.hpp>
#include <restpp/core/http_headers.hpp>
#include <restpp/core/uri.hpp>

namespace restpp
{
namespace details
{
/// <summary>
/// Internal representation of an HTTP request message.
/// </summary>
class _http_request final : public details::http_msg_base, public std::enable_shared_from_this<_http_request>
{
public:
    _ASYNCRTIMP _http_request(method mtd);

    _ASYNCRTIMP _http_request(std::unique_ptr<details::_http_server_context> server_context);

    virtual ~_http_request() {}

    method& method() { return _method; }

    uri& request_uri() { return _uri; }

    _ASYNCRTIMP uri absolute_uri() const;

    _ASYNCRTIMP uri relative_uri() const;

    _ASYNCRTIMP void set_request_uri(const uri&);

    const utility::string_t& remote_address() const { return _remote_address; }

    utility::string_t path() { return _uri.path(); }

    // TODO: need to implement this.
    std::map<utility::string_t, utility::string_t> query() { throw std::exception{}; }

    // TODO: need to implement this.
    std::map<utility::string_t, utility::string_t> params() { throw std::exception{}; }

    const pplx::cancellation_token& cancellation_token() const { return _cancellationToken; }

    void set_cancellation_token(const pplx::cancellation_token& token) { _cancellationToken = token; }

    _ASYNCRTIMP utility::string_t to_string() const;

    pplx::task<http_response> get_response() { return pplx::task<http_response>(_response); }

    void set_response_stream(const concurrency::streams::ostream& stream) { _response_stream = stream; }

    void set_progress_handler(const progress_handler& handler)
    {
        _progress_handler = std::make_shared<progress_handler>(handler);
    }

    const concurrency::streams::ostream& _response_stream() const { return _response_stream; }

    const std::shared_ptr<progress_handler>& _progress_handler() const { return _progress_handler; }

    details::_http_server_context* _get_server_context() const { return _server_context.get(); }

    void _set_server_context(std::unique_ptr<details::_http_server_context> server_context)
    {
        _server_context = std::move(server_context);
    }

    void _set_listener_path(const utility::string_t& path) { _listener_path = path; }

    void _set_base_uri(const uri& base_uri) { _base_uri = base_uri; }

    void _set_remote_address(const utility::string_t& remote_address) { _remote_address = remote_address; }

private:
    // Actual initiates sending the response, without checking if a response has already been sent.
    pplx::task<void> _reply_impl(http_response response);

    method _method;

    // Tracks whether or not a response has already been started for this message.
    // 0 = No reply sent
    // 1 = Usual reply sent
    // 2 = Reply aborted by another thread; e.g. server shutdown
    pplx::details::atomic_long _initiated_response;

    std::unique_ptr<details::_http_server_context> _server_context;

    pplx::cancellation_token _cancellationToken;

    uri _base_uri;
    uri _uri;
    utility::string_t _listener_path;

    concurrency::streams::ostream _response_stream;

    std::shared_ptr<progress_handler> _progress_handler;

    pplx::task_completion_event<http_response> _response;

    utility::string_t _remote_address;
};
} // namespace details

/// <summary>
/// Represents an HTTP request.
/// </summary>
class http_request
{
public:
    /// <summary>
    /// Constructs a new HTTP request with the 'GET' method.
    /// </summary>
    http_request() : _impl(std::make_shared<details::_http_request>(methods::GET)) {}

    /// <summary>
    /// Constructs a new HTTP request with the given request method.
    /// </summary>
    /// <param name="mtd">Request method.</param>
    http_request(const uri& uri, method mtd = methods::GET) : _impl(std::make_shared<details::_http_request>(std::move(mtd)))
    {
        _impl->set_request_uri(uri);
    }

    /// <summary>
    /// Destructor frees any held resources.
    /// </summary>
    ~http_request() {}

    /// <summary>
    /// Get the method (GET/PUT/POST/DELETE) of the request message.
    /// </summary>
    /// <returns>Request method of this HTTP request.</returns>
    const method& method() const { return _impl->method(); }

    /// <summary>
    /// Set the method (GET/PUT/POST/DELETE) of the request message.
    /// </summary>
    /// <param name="method">Request method of this HTTP request.</param>
    void set_method(const method& method) const { _impl->method() = method; }

    /// <summary>
    /// Get the underling URI of the request message.
    /// </summary>
    /// <returns>The uri of this message.</returns>
    const uri& request_uri() const { return _impl->request_uri(); }

    /// <summary>
    /// Set the underling URI of the request message.
    /// </summary>
    /// <param name="uri">The uri for this message.</param>
    void set_request_uri(const uri& uri) { return _impl->set_request_uri(uri); }

    /// <summary>
    /// Gets a reference the URI path, query, and fragment part of this request message.
    /// This will be appended to the base URI specified at construction of the http_client.
    /// </summary>
    /// <returns>A string.</returns>
    /// <remarks>When the request is the one passed to a listener's handler, the
    /// relative URI is the request URI less the listener's path. In all other circumstances,
    /// request_uri() and relative_uri() will return the same value.
    /// </remarks>
    uri relative_uri() const { return _impl->relative_uri(); }

    /// <summary>
    /// Get an absolute URI with scheme, host, port, path, query, and fragment part of
    /// the request message.
    /// </summary>
    /// <remarks>Absolute URI is only valid after this http_request object has been passed
    /// to http_client::request().
    /// </remarks>
    uri absolute_uri() const { return _impl->absolute_uri(); }

    /// <summary>
    /// Get the path component of the URI as an encoded string.
    /// </summary>
    /// <returns>The URI path as a string.</returns>
    /// <remarks>
    /// Query parameters are parsed from the URI of the request.
    /// </remarks>
    utility::string_t path() const { _impl->path(); }

    /// <summary>
    /// Retrieves the query parameters from the request URI.
    /// </summary>
    /// <returns>
    /// A map of query parameter keys and their corresponding values.
    /// </returns>
    /// <remarks>
    /// Query parameters are parsed from the URI of the request.
    /// </remarks>
    std::map<utility::string_t, utility::string_t> query() const { return _impl->query(); }

    /// <summary>
    /// Retrieves the form or URL-encoded parameters from the request body.
    /// </summary>
    /// <returns>
    /// A map of parameter keys and their corresponding values.
    /// </returns>
    /// <remarks>
    /// This method is typically used for requests with a body, such as POST or PUT, where
    /// parameters are encoded in the request payload.
    /// </remarks>
    std::map<utility::string_t, utility::string_t> params() const { return _impl->params(); }

    /// <summary>
    /// Gets a reference to the headers of the response message.
    /// </summary>
    /// <returns>HTTP headers for this response.</returns>
    /// <remarks>
    /// Use the http_headers::add to fill in desired headers.
    /// </remarks>
    http_headers& headers() { return _impl->headers(); }

    /// <summary>
    /// Gets a const reference to the headers of the response message.
    /// </summary>
    /// <returns>HTTP headers for this response.</returns>
    /// <remarks>
    /// Use the http_headers::add to fill in desired headers.
    /// </remarks>
    const http_headers& headers() const { return _impl->headers(); }

    /// <summary>
    /// Returns the HTTP protocol version of this request message.
    /// </summary>
    /// <returns>The HTTP protocol version.</returns>
    http_version http_version() const { return _impl->http_version(); }

    /// <summary>
    /// Returns a string representation of the remote IP address.
    /// </summary>
    /// <returns>The remote IP address.</returns>
    const utility::string_t& remote_address() const { return _impl->remote_address(); }

    /// <summary>
    /// Extract the body of the request message as a string value, checking that the content type is a MIME text type.
    /// A body can only be extracted once because in some cases an optimization is made where the data is 'moved' out.
    /// </summary>
    /// <param name="ignore_content_type">If true, ignores the Content-Type header and assumes UTF-8.</param>
    /// <returns>String containing body of the message.</returns>
    // pplx::task<utility::string_t> extract_string(bool ignore_content_type = false)
    // {
    //     auto impl = _impl;
    //     return pplx::create_task(_impl->_get_data_available()).then([impl, ignore_content_type](utility::size64_t) {
    //         return impl->extract_string(ignore_content_type);
    //     });
    // }

    /// <summary>
    /// Extract the body of the request message as a UTF-8 string value, checking that the content type is a MIME text
    /// type. A body can only be extracted once because in some cases an optimization is made where the data is 'moved'
    /// out.
    /// </summary>
    /// <param name="ignore_content_type">If true, ignores the Content-Type header and assumes UTF-8.</param>
    /// <returns>String containing body of the message.</returns>
    // pplx::task<utf8string> extract_utf8string(bool ignore_content_type = false)
    // {
    //     auto impl = _impl;
    //     return pplx::create_task(_impl->_get_data_available()).then([impl, ignore_content_type](utility::size64_t) {
    //         return impl->extract_utf8string(ignore_content_type);
    //     });
    // }

    /// <summary>
    /// Extract the body of the request message as a UTF-16 string value, checking that the content type is a MIME text
    /// type. A body can only be extracted once because in some cases an optimization is made where the data is 'moved'
    /// out.
    /// </summary>
    /// <param name="ignore_content_type">If true, ignores the Content-Type header and assumes UTF-16.</param>
    /// <returns>String containing body of the message.</returns>
    // pplx::task<utf16string> extract_utf16string(bool ignore_content_type = false)
    // {
    //     auto impl = _impl;
    //     return pplx::create_task(_impl->_get_data_available()).then([impl, ignore_content_type](utility::size64_t) {
    //         return impl->extract_utf16string(ignore_content_type);
    //     });
    // }

    /// <summary>
    /// Extracts the body of the request message into a json value, checking that the content type is application/json.
    /// A body can only be extracted once because in some cases an optimization is made where the data is 'moved' out.
    /// </summary>
    /// <param name="ignore_content_type">If true, ignores the Content-Type header and assumes UTF-8.</param>
    /// <returns>JSON value from the body of this message.</returns>
    // pplx::task<json::value> extract_json(bool ignore_content_type = false) const
    // {
    //     auto impl = _impl;
    //     return pplx::create_task(_impl->_get_data_available()).then([impl, ignore_content_type](utility::size64_t) {
    //         return impl->_extract_json(ignore_content_type);
    //     });
    // }

    /// <summary>
    /// Extract the body of the response message into a vector of bytes. Extracting a vector can be done on
    /// </summary>
    /// <returns>The body of the message as a vector of bytes.</returns>
    // pplx::task<std::vector<unsigned char>> extract_vector() const
    // {
    //     auto impl = _impl;
    //     return pplx::create_task(_impl->_get_data_available()).then([impl](utility::size64_t) {
    //         return impl->_extract_vector();
    //     });
    // }

    /// <summary>
    /// Sets the body of the message to a textual string and set the "Content-Type" header. Assumes
    /// the character encoding of the string is UTF-8.
    /// </summary>
    /// <param name="body_text">String containing body text.</param>
    /// <param name="content_type">MIME type to set the "Content-Type" header to. Default to "text/plain;
    /// charset=utf-8".</param> <remarks> This will overwrite any previously set body data and "Content-Type" header.
    /// </remarks>
    // void set_body(utf8string&& body_text, const utf8string& content_type = utf8string("text/plain; charset=utf-8"))
    // {
    //     const auto length = body_text.size();
    //     _impl->set_body(
    //         concurrency::streams::bytestream::open_istream<std::string>(std::move(body_text)), length, content_type);
    // }

    /// <summary>
    /// Sets the body of the message to a textual string and set the "Content-Type" header. Assumes
    /// the character encoding of the string is UTF-8.
    /// </summary>
    /// <param name="body_text">String containing body text.</param>
    /// <param name="content_type">MIME type to set the "Content-Type" header to. Default to "text/plain;
    /// charset=utf-8".</param> <remarks> This will overwrite any previously set body data and "Content-Type" header.
    /// </remarks>
    // void set_body(const utf8string& body_text, const utf8string& content_type = utf8string("text/plain; charset=utf-8"))
    // {
    //     _impl->set_body(
    //         concurrency::streams::bytestream::open_istream<std::string>(body_text), body_text.size(), content_type);
    // }

    /// <summary>
    /// Sets the body of the message to a textual string and set the "Content-Type" header. Assumes
    /// the character encoding of the string is UTF-16 will perform conversion to UTF-8.
    /// </summary>
    /// </summary>
    /// <param name="body_text">String containing body text.</param>
    /// <param name="content_type">MIME type to set the "Content-Type" header to. Default to "text/plain".</param>
    /// <remarks>
    /// This will overwrite any previously set body data and "Content-Type" header.
    /// </remarks>
    // void set_body(const utf16string& body_text,
    //               utf16string content_type = utility::conversions::to_utf16string("text/plain"))
    // {
    //     if (content_type.find(::utility::conversions::to_utf16string("charset=")) != content_type.npos)
    //     {
    //         throw std::invalid_argument("content_type can't contain a 'charset'.");
    //     }

    //     auto utf8body = utility::conversions::utf16_to_utf8(body_text);
    //     auto length = utf8body.size();
    //     _impl->set_body(concurrency::streams::bytestream::open_istream(std::move(utf8body)),
    //                       length,
    //                       std::move(content_type.append(::utility::conversions::to_utf16string("; charset=utf-8"))));
    // }

    /// <summary>
    /// Sets the body of the message to contain json value. If the 'Content-Type'
    /// header hasn't already been set it will be set to 'application/json'.
    /// </summary>
    /// <param name="body_data">json value.</param>
    /// <remarks>
    /// This will overwrite any previously set body data.
    /// </remarks>
    // void set_body(const json::value& body_data)
    // {
    //     auto body_text = utility::conversions::to_utf8string(body_data.serialize());
    //     auto length = body_text.size();
    //     _impl->set_body(concurrency::streams::bytestream::open_istream(std::move(body_text)),
    //                       length,
    //                       _XPLATSTR("application/json"));
    // }

    /// <summary>
    /// Sets the body of the message to the contents of a byte vector. If the 'Content-Type'
    /// header hasn't already been set it will be set to 'application/octet-stream'.
    /// </summary>
    /// <param name="body_data">Vector containing body data.</param>
    /// <remarks>
    /// This will overwrite any previously set body data.
    /// </remarks> void set_body(std::vector<unsigned char>&& body_data) { auto length = body_data.size(); _impl->set_body(concurrency::streams::bytestream::open_istream(std::move(body_data)), length, _XPLATSTR("application/octet-stream")); }

    /// <summary>
    /// Sets the body of the message to the contents of a byte vector. If the 'Content-Type'
    /// header hasn't already been set it will be set to 'application/octet-stream'.
    /// </summary>
    /// <param name="body_data">Vector containing body data.</param>
    /// <remarks>
    /// This will overwrite any previously set body data.
    /// </remarks>
    // void set_body(const std::vector<unsigned char>& body_data)
    // {
    //     set_body(concurrency::streams::bytestream::open_istream(body_data), body_data.size());
    // }

    /// <summary>
    /// Defines a stream that will be relied on to provide the body of the HTTP message when it is
    /// sent.
    /// </summary>
    /// <param name="stream">A readable, open asynchronous stream.</param>
    /// <param name="content_type">A string holding the MIME type of the message body.</param>
    /// <remarks>
    /// This cannot be used in conjunction with any other means of setting the body of the request.
    /// The stream will not be read until the message is sent.
    /// </remarks>
    // void set_body(const concurrency::streams::istream& stream,
    //               const utility::string_t& content_type = _XPLATSTR("application/octet-stream"))
    // {
    //     _impl->set_body(stream, content_type);
    // }

    /// <summary>
    /// Defines a stream that will be relied on to provide the body of the HTTP message when it is
    /// sent.
    /// </summary>
    /// <param name="stream">A readable, open asynchronous stream.</param>
    /// <param name="content_length">The size of the data to be sent in the body.</param>
    /// <param name="content_type">A string holding the MIME type of the message body.</param>
    /// <remarks>
    /// This cannot be used in conjunction with any other means of setting the body of the request.
    /// The stream will not be read until the message is sent.
    /// </remarks>
    // void set_body(const concurrency::streams::istream& stream,
    //               utility::size64_t content_length,
    //               const utility::string_t& content_type = _XPLATSTR("application/octet-stream"))
    // {
    //     _impl->set_body(stream, content_length, content_type);
    // }

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
    concurrency::streams::istream body() const { return _impl->instream(); }

    /// <summary>
    /// Defines a stream that will be relied on to hold the body of the HTTP response message that
    /// results from the request.
    /// </summary>
    /// <param name="stream">A writable, open asynchronous stream.</param>
    /// <remarks>
    /// If this function is called, the body of the response should not be accessed in any other
    /// way.
    /// </remarks>
    void set_response_stream(const concurrency::streams::ostream& stream)
    {
        return _impl->set_response_stream(stream);
    }

    /// <summary>
    /// Sets a compressor that will be used to compress the body of the HTTP message as it is sent.
    /// </summary>
    /// <param name="compressor">A pointer to an instantiated compressor of the desired type.</param>
    /// <remarks>
    /// This cannot be used in conjunction with any external means of compression.  The Transfer-Encoding
    /// header will be managed internally, and must not be set by the client.
    /// </remarks>
    void set_compressor(std::unique_ptr<compression::compress_provider> compressor)
    {
        return _impl->set_compressor(std::move(compressor));
    }

    /// <summary>
    /// Sets a compressor that will be used to compress the body of the HTTP message as it is sent.
    /// </summary>
    /// <param name="algorithm">The built-in compression algorithm to use.</param>
    /// <returns>
    /// True if a built-in compressor was instantiated, otherwise false.
    /// </returns>
    /// <remarks>
    /// This cannot be used in conjunction with any external means of compression.  The Transfer-Encoding
    /// header will be managed internally, and must not be set by the client.
    /// </remarks>
    bool set_compressor(utility::string_t algorithm)
    {
        _impl->set_compressor(compression::builtin::make_compressor(algorithm));
        return (bool)_impl->compressor();
    }

    /// <summary>
    /// Gets the compressor to be used to compress the message body, if any.
    /// </summary>
    /// <returns>
    /// The compressor itself.
    /// </returns>
    std::unique_ptr<compression::compress_provider>& compressor() { return _impl->compressor(); }

    /// <summary>
    /// Sets the default collection of built-in factory classes for decompressors that may be used to
    /// decompress the body of the HTTP message as it is received, effectively enabling decompression.
    /// </summary>
    /// <param name="factories">The collection of factory classes for allowable decompressors. The
    /// supplied vector itself need not remain valid after the call returns.</param>
    /// <remarks>
    /// This default collection is implied if request_compressed_response() is set in the associated
    /// <c>client::http_client_config</c> and neither overload of this method has been called.
    ///
    /// This cannot be used in conjunction with any external means of decompression.  The TE and Accept-Encoding
    /// headers must not be set by the client, as they will be managed internally as appropriate.
    /// </remarks>
    _ASYNCRTIMP void set_decompress_factories();

    /// <summary>
    /// Sets a collection of factory classes for decompressors that may be used to decompress the
    /// body of the HTTP message as it is received, effectively enabling decompression.
    /// </summary>
    /// <remarks>
    /// If set, this collection takes the place of the built-in compression providers.  It may contain
    /// custom factory classes and/or factory classes for built-in providers, and may be used to adjust
    /// the weights of the built-in providers, which default to 500 (i.e. "q=0.500").
    ///
    /// This cannot be used in conjunction with any external means of decompression.  The TE and Accept-Encoding
    /// headers must not be set by the client, as they will be managed internally as appropriate.
    /// </remarks>
    void set_decompress_factories(const std::vector<std::shared_ptr<compression::decompress_factory>>& factories)
    {
        return _impl->set_decompress_factories(factories);
    }

    /// <summary>
    /// Gets the collection of factory classes for decompressors to be used to decompress the message body, if any.
    /// </summary>
    /// <returns>
    /// The collection of factory classes itself.
    /// </returns>
    /// <remarks>
    /// This cannot be used in conjunction with any external means of decompression.  The TE
    /// header must not be set by the client, as it will be managed internally.
    /// </remarks>
    const std::vector<std::shared_ptr<compression::decompress_factory>>& decompress_factories() const
    {
        return _impl->decompress_factories();
    }

    /// <summary>
    /// Defines a callback function that will be invoked for every chunk of data uploaded or downloaded
    /// as part of the request.
    /// </summary>
    /// <param name="handler">A function representing the progress handler. It's parameters are:
    ///    up:       a <c>message_direction::direction</c> value  indicating the direction of the message
    ///              that is being reported.
    ///    progress: the number of bytes that have been processed so far.
    /// </param>
    /// <remarks>
    ///   This function will be called at least once for upload and at least once for
    ///   the download body, unless there is some exception generated. An HTTP message with an error
    ///   code is not an exception. This means, that even if there is no body, the progress handler
    ///   will be called.
    ///
    ///   Setting the chunk size on the http_client does not guarantee that the client will be using
    ///   exactly that increment for uploading and downloading data.
    ///
    ///   The handler will be called only once for each combination of argument values, in order. Depending
    ///   on how a service responds, some download values may come before all upload values have been
    ///   reported.
    ///
    ///   The progress handler will be called on the thread processing the request. This means that
    ///   the implementation of the handler must take care not to block the thread or do anything
    ///   that takes significant amounts of time. In particular, do not do any kind of I/O from within
    ///   the handler, do not update user interfaces, and to not acquire any locks. If such activities
    ///   are necessary, it is the handler's responsibility to execute that work on a separate thread.
    /// </remarks>
    void set_progress_handler(const progress_handler& handler) { return _impl->set_progress_handler(handler); }

    /// <summary>
    /// Signals the user (listener) when all the data for this request message has been received.
    /// </summary>
    /// <returns>A <c>task</c> which is completed when all of the response body has been received</returns>
    pplx::task<http_request> content_ready() const
    {
        http_request req = *this;
        return pplx::create_task(_impl->_get_data_available()).then([req](utility::size64_t) mutable { return req; });
    }

    /// <summary>
    /// Gets a task representing the response that will eventually be sent.
    /// </summary>
    /// <returns>A task that is completed once response is sent.</returns>
    pplx::task<http_response> get_response() const { return _impl->get_response(); }

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

    /// <summary>
    /// Gets the server context associated with this HTTP message.
    /// </summary>
    details::_http_server_context* _get_server_context() const { return _impl->_get_server_context(); }

    /// <summary>
    /// These functions are used for the initial creation of the HTTP request.
    /// </summary>
    static http_request _create_request(std::unique_ptr<details::_http_server_context> server_context)
    {
        return http_request(std::move(server_context));
    }
    void _set_server_context(std::unique_ptr<details::_http_server_context> server_context)
    {
        _impl->_set_server_context(std::move(server_context));
    }

    void _set_listener_path(const utility::string_t& path) { _impl->_set_listener_path(path); }

    const std::shared_ptr<details::_http_request>& _get_impl() const { return _impl; }

    void _set_cancellation_token(const pplx::cancellation_token& token) { _impl->set_cancellation_token(token); }

    const pplx::cancellation_token& _cancellation_token() const { return _impl->cancellation_token(); }

    void _set_base_uri(const uri& base_uri) { _impl->_set_base_uri(base_uri); }

private:
    friend class details::_http_request;

    http_request(std::unique_ptr<details::_http_server_context> server_context)
        : _impl(std::make_shared<details::_http_request>(std::move(server_context)))
    {
    }

    std::shared_ptr<details::_http_request> _impl;
};
} // namespace restpp

#endif // RESTPP_HTTP_REQUEST_HPP