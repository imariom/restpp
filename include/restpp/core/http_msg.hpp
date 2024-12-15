/**
 * Copyright (C) 2024 Mário A. Moiane (connect at imariom dot com)
 * Licensed under the MIT license. See LICENSE.txt in the project root for details.
 *
 * HTTP Library: Request and reply message definitions.
 * 
 * For more information and updates, visit: https://github.com/imariom/restpp
 */

#pragma once
#ifndef RESTPP_HTTP_MSG_HPP
#define RESTPP_HTTP_MSG_HPP

#include <functional>
#include <cstdint>

#include <restpp/core/details/basic_types.hpp>

namespace restpp
{
/// <summary>
/// Represents the HTTP protocol version of a message, as {major, minor}.
/// </summary>
struct http_version
{
    uint8_t major;
    uint8_t minor;

    inline bool operator==(const http_version& other) const { return major == other.major && minor == other.minor; }
    inline bool operator<(const http_version& other) const
    {
        return major < other.major || (major == other.major && minor < other.minor);
    }

    inline bool operator!=(const http_version& other) const { return !(*this == other); }
    inline bool operator>=(const http_version& other) const { return !(*this < other); }
    inline bool operator>(const http_version& other) const { return !(*this < other || *this == other); }
    inline bool operator<=(const http_version& other) const { return *this < other || *this == other; }

    /// <summary>
    /// Creates <c>http_version</c> from an HTTP-Version string, "HTTP" "/" 1*DIGIT "." 1*DIGIT.
    /// </summary>
    /// <returns>Returns a <c>http_version</c> of {0, 0} if not successful.</returns>
    static _ASYNCRTIMP http_version __cdecl from_string(const std::string& http_version_string)
    {
        std::istringstream str(http_version_string);
        str.imbue(std::locale::classic());

        std::string http;
        std::getline(str, http, '/');
        unsigned int major = 0;
        str >> major;
        char dot = '\0';
        str >> dot;
        unsigned int minor = 0;
        str >> minor;

        // check no failure, fully consumed, and correct fixed text
        if (!str.fail() && str.eof() && "HTTP" == http && '.' == dot)
        {
            return {(uint8_t)major, (uint8_t)minor};
        }
        return {0, 0};
    }

    /// <summary>
    /// Returns the string representation of the <c>http_version</c>.
    /// </summary>
    _ASYNCRTIMP std::string to_utf8string() const
    {
        std::string ret;
        ret.reserve(8);
        ret.append("HTTP/");
        ret.append(std::to_string(static_cast<unsigned int>(major)));
        ret.append(".");
        ret.append(std::to_string(static_cast<unsigned int>(minor)));
        return ret;
    }
};

/// <summary>
/// Predefined HTTP protocol versions.
/// </summary>
class http_versions
{
public:
    _ASYNCRTIMP static const http_version HTTP_0_9;
    _ASYNCRTIMP static const http_version HTTP_1_0;
    _ASYNCRTIMP static const http_version HTTP_1_1;
};

/// <summary>
/// Predefined method strings for the standard HTTP methods mentioned in the
/// HTTP 1.1 specification.
/// </summary>
typedef utility::string_t method;

/// <summary>
/// Common HTTP methods.
/// </summary>
class methods
{
public:
#define _METHODS
#define DAT(a, b) _ASYNCRTIMP const static method a;
#include <restpp/core/details/http_constants.dat>
#undef _METHODS
#undef DAT
};

typedef unsigned short status_code;

/// <summary>
/// Predefined values for all of the standard HTTP 1.1 response status codes.
/// </summary>
class status_codes
{
public:
#define _PHRASES
#define DAT(a, b, c) const static status_code a = b;
#include <restpp/core/details/http_constants.dat>
#undef _PHRASES
#undef DAT
};

namespace details
{
/// <summary>
/// Constants for MIME types.
/// </summary>
class mime_types
{
public:
#define _MIME_TYPES
#define DAT(a, b) _ASYNCRTIMP const static utility::string_t a;
#include <restpp/core/details/http_constants.dat>
#undef _MIME_TYPES
#undef DAT
};

/// <summary>
/// Constants for charset types.
/// </summary>
class charset_types
{
public:
#define _CHARSET_TYPES
#define DAT(a, b) _ASYNCRTIMP const static utility::string_t a;
#include <restpp/core/details/http_constants.dat>
#undef _CHARSET_TYPES
#undef DAT
};
} // namespace details

/// Message direction
namespace message_direction
{
/// <summary>
/// Enumeration used to denote the direction of a message: a request with a body
/// and a response in the context of of an http_server is an upload.
/// A response with a body is a download in the context of the fetch API.
/// </summary>
enum direction
{
    upload,
    download
};
} // namespace message_direction

using reason_phrase = utility::string_t;
using progress_handler = std::function<void(message_direction::direction, utility::size64_t)>;

struct http_status_to_phrase
{
    unsigned short id;
    reason_phrase phrase;
};

/// <summary>
/// Constants for the HTTP headers mentioned in RFC 2616.
/// </summary>
class header_names
{
public:
#define _HEADER_NAMES
#define DAT(a, b) _ASYNCRTIMP const static utility::string_t a;
#include <restpp/core/details/http_constants.dat>
#undef _HEADER_NAMES
#undef DAT
};

/// <summary>
/// Represents an HTTP error. This class holds an error message and an optional error code.
/// </summary>
class _ASYNCRTIMP_TYPEINFO http_exception : public std::exception
{
public:
    /// <summary>
    /// Creates an <c>http_exception</c> with just a string message and no error code.
    /// </summary>
    /// <param name="whatArg">Error message string.</param>
    http_exception(const utility::string_t& whatArg) : _msg(utility::conversions::to_utf8string(whatArg)) {}

#ifdef _WIN32
    /// <summary>
    /// Creates an <c>http_exception</c> with just a string message and no error code.
    /// </summary>
    /// <param name="whatArg">Error message string.</param>
    http_exception(std::string whatArg) : _msg(std::move(whatArg)) {}
#endif

    /// <summary>
    /// Creates an <c>http_exception</c> with from a error code using the current platform error category.
    /// The message of the error code will be used as the what() string message.
    /// </summary>
    /// <param name="errorCode">Error code value.</param>
    http_exception(int errorCode) : _errorCode(utility::details::create_error_code(errorCode))
    {
        _msg = _errorCode.message();
    }

    /// <summary>
    /// Creates an <c>http_exception</c> with from a error code using the current platform error category.
    /// </summary>
    /// <param name="errorCode">Error code value.</param>
    /// <param name="whatArg">Message to use in what() string.</param>
    http_exception(int errorCode, const utility::string_t& whatArg)
        : _errorCode(utility::details::create_error_code(errorCode))
        , _msg(utility::conversions::to_utf8string(whatArg))
    {
    }

#ifdef _WIN32
    /// <summary>
    /// Creates an <c>http_exception</c> with from a error code using the current platform error category.
    /// </summary>
    /// <param name="errorCode">Error code value.</param>
    /// <param name="whatArg">Message to use in what() string.</param>
    http_exception(int errorCode, std::string whatArg)
        : _errorCode(utility::details::create_error_code(errorCode)), _msg(std::move(whatArg))
    {
    }
#endif

    /// <summary>
    /// Creates an <c>http_exception</c> with from a error code and category. The message of the error code will be used
    /// as the <c>what</c> string message.
    /// </summary>
    /// <param name="errorCode">Error code value.</param>
    /// <param name="cat">Error category for the code.</param>
    http_exception(int errorCode, const std::error_category& cat) : _errorCode(std::error_code(errorCode, cat))
    {
        _msg = _errorCode.message();
    }

    /// <summary>
    /// Creates an <c>http_exception</c> with from a error code with a category, and a string message.
    /// </summary>
    /// <param name="errorCode">Error code value.</param>
    /// <param name="whatArg">Error message string.</param>
    http_exception(std::error_code errorCode, const utility::string_t& whatArg)
        : _errorCode(std::move(errorCode)), _msg(utility::conversions::to_utf8string(whatArg))
    {
    }

#ifdef _WIN32
    /// <summary>
    /// Creates an <c>http_exception</c> with from a error code with a category, and a string message.
    /// </summary>
    /// <param name="errorCode">Error code value.</param>
    /// <param name="whatArg">Error message string.</param>
    http_exception(std::error_code errorCode, std::string whatArg)
        : _errorCode(std::move(errorCode)), _msg(std::move(whatArg))
    {
    }
#endif

    /// <summary>
    /// Gets a string identifying the cause of the exception.
    /// </summary>
    /// <returns>A null terminated character string.</returns>
    const char* what() const RESTPP_NOEXCEPT { return _msg.c_str(); }

    /// <summary>
    /// Retrieves the underlying error code causing this exception.
    /// </summary>
    /// <returns>A std::error_code.</returns>
    const std::error_code& error_code() const { return _errorCode; }

private:
    std::error_code _errorCode;
    std::string _msg;
};

namespace details
{
/// <summary>
/// Base class for HTTP messages.
/// This class is to store common functionality so it isn't duplicated on
/// both the request and response side.
/// </summary>
class http_msg_base
{
public:
    friend class http::client::http_client;

    _ASYNCRTIMP http_msg_base();

    virtual ~http_msg_base() {}

    http::http_version http_version() const { return m_http_version; }

    http_headers& headers() { return m_headers; }

    _ASYNCRTIMP void set_body(const concurrency::streams::istream& instream, const utf8string& contentType);
    _ASYNCRTIMP void set_body(const concurrency::streams::istream& instream, const utf16string& contentType);
    _ASYNCRTIMP void set_body(const concurrency::streams::istream& instream,
                              utility::size64_t contentLength,
                              const utf8string& contentType);
    _ASYNCRTIMP void set_body(const concurrency::streams::istream& instream,
                              utility::size64_t contentLength,
                              const utf16string& contentType);

    /// <summary>
    /// Helper function for extract functions. Parses the Content-Type header and check to make sure it matches,
    /// throws an exception if not.
    /// </summary>
    /// <param name="ignore_content_type">If true ignores the Content-Type header value.</param>
    /// <param name="check_content_type">Function to verify additional information on Content-Type.</param>
    /// <returns>A string containing the charset, an empty string if no Content-Type header is empty.</returns>
    utility::string_t parse_and_check_content_type(
        bool ignore_content_type, const std::function<bool(const utility::string_t&)>& check_content_type);

    _ASYNCRTIMP utf8string extract_utf8string(bool ignore_content_type = false);
    _ASYNCRTIMP utf16string extract_utf16string(bool ignore_content_type = false);
    _ASYNCRTIMP utility::string_t extract_string(bool ignore_content_type = false);

    _ASYNCRTIMP json::value _extract_json(bool ignore_content_type = false);
    _ASYNCRTIMP std::vector<unsigned char> _extract_vector();

    virtual _ASYNCRTIMP utility::string_t to_string() const;

    /// <summary>
    /// Completes this message
    /// </summary>
    virtual _ASYNCRTIMP void _complete(utility::size64_t bodySize,
                                       const std::exception_ptr& exceptionPtr = std::exception_ptr());

    /// <summary>
    /// Set the stream through which the message body could be read
    /// </summary>
    void set_instream(const concurrency::streams::istream& instream) { m_inStream = instream; }

    /// <summary>
    /// Get the stream through which the message body could be read
    /// </summary>
    const concurrency::streams::istream& instream() const { return m_inStream; }

    /// <summary>
    /// Set the stream through which the message body could be written
    /// </summary>
    void set_outstream(const concurrency::streams::ostream& outstream, bool is_default)
    {
        m_outStream = outstream;
        m_default_outstream = is_default;
    }

    /// <summary>
    /// Get the stream through which the message body could be written
    /// </summary>
    const concurrency::streams::ostream& outstream() const { return m_outStream; }

    /// <summary>
    /// Sets the compressor for the message body
    /// </summary>
    void set_compressor(std::unique_ptr<http::compression::compress_provider> compressor)
    {
        m_compressor = std::move(compressor);
    }

    /// <summary>
    /// Gets the compressor for the message body, if any
    /// </summary>
    std::unique_ptr<http::compression::compress_provider>& compressor() { return m_compressor; }

    /// <summary>
    /// Sets the collection of factory classes for decompressors for use with the message body
    /// </summary>
    void set_decompress_factories(const std::vector<std::shared_ptr<http::compression::decompress_factory>>& factories)
    {
        m_decompressors = factories;
    }

    /// <summary>
    /// Gets the collection of factory classes for decompressors to be used to decompress the message body, if any
    /// </summary>
    const std::vector<std::shared_ptr<http::compression::decompress_factory>>& decompress_factories()
    {
        return m_decompressors;
    }

    const pplx::task_completion_event<utility::size64_t>& _get_data_available() const { return m_data_available; }

    /// <summary>
    /// Prepare the message with an output stream to receive network data
    /// </summary>
    _ASYNCRTIMP void _prepare_to_receive_data();

    /// <summary>
    /// Determine the remaining input stream length
    /// </summary>
    /// <returns>
    /// std::numeric_limits<size_t>::max() if the stream's remaining length cannot be determined
    /// length      if the stream's remaining length (which may be 0) can be determined
    /// </returns>
    /// <remarks>
    /// This routine should only be called after a msg (request/response) has been
    /// completely constructed.
    /// </remarks>
    _ASYNCRTIMP size_t _get_stream_length();

    /// <summary>
    /// Determine the content length
    /// </summary>
    /// <returns>
    /// std::numeric_limits<size_t>::max() if there is content with unknown length (transfer_encoding:chunked)
    /// 0           if there is no content
    /// length      if there is content with known length
    /// </returns>
    /// <remarks>
    /// This routine should only be called after a msg (request/response) has been
    /// completely constructed.
    /// </remarks>
    _ASYNCRTIMP size_t _get_content_length();

    /// <summary>
    /// Determine the content length, and, if necessary, manage compression in the Transfer-Encoding header
    /// </summary>
    /// <returns>
    /// std::numeric_limits<size_t>::max() if there is content with unknown length (transfer_encoding:chunked)
    /// 0           if there is no content
    /// length      if there is content with known length
    /// </returns>
    /// <remarks>
    /// This routine is like _get_content_length, except that it adds a compression algorithm to
    /// the Trasfer-Length header if compression is configured.  It throws if a Transfer-Encoding
    /// header exists and does not match the one it generated.
    /// </remarks>
    _ASYNCRTIMP size_t _get_content_length_and_set_compression();

    void _set_http_version(const http::http_version& http_version) { m_http_version = http_version; }

protected:
    std::unique_ptr<http::compression::compress_provider> m_compressor;
    std::unique_ptr<http::compression::decompress_provider> m_decompressor;
    std::vector<std::shared_ptr<http::compression::decompress_factory>> m_decompressors;

    /// <summary>
    /// Stream to read the message body.
    /// By default this is an invalid stream. The user could set the instream on
    /// a request by calling set_request_stream(...). This would also be set when
    /// set_body() is called - a stream from the body is constructed and set.
    /// Even in the presence of msg body this stream could be invalid. An example
    /// would be when the user sets an ostream for the response. With that API the
    /// user does not provide the ability to read the msg body.
    /// Thus m_instream is valid when there is a msg body and it can actually be read
    /// </summary>
    concurrency::streams::istream m_inStream;

    /// <summary>
    /// stream to write the msg body
    /// By default this is an invalid stream. The user could set this on the response
    /// (for http_client). In all the other cases we would construct one to transfer
    /// the data from the network into the message body.
    /// </summary>
    concurrency::streams::ostream m_outStream;

    http::http_version m_http_version;
    http_headers m_headers;
    bool m_default_outstream;

    /// <summary> The TCE is used to signal the availability of the message body. </summary>
    pplx::task_completion_event<utility::size64_t> m_data_available;

    size_t _get_content_length(bool honor_compression);
};

/// <summary>
/// Base structure for associating internal server information
/// with an HTTP request/response.
/// </summary>
class _http_server_context
{
public:
    _http_server_context() {}
    virtual ~_http_server_context() {}

private:
};
} // namespace details
} // namespace restpp

#endif // RESTPP_HTTP_MSG_HPP