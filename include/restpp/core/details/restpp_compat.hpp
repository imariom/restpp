/***
 * Copyright (C) 2024-present Mário A. Moiane (connect at imariom dot com)
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * Standard macros and definitions.
 * This header has minimal dependency on windows headers and is safe for use in the public API
 *
 * For the latest on this and related APIs, please see: https://github.com/imariom/restpp
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 ****/

#pragma once
#ifndef RESTPP_COMPAT_HPP
#define RESTPP_COMPAT_HPP 

#if defined(_WIN32)

#if _MSC_VER >= 1900
#define RESTPP_NOEXCEPT noexcept
#define RESTPP_CONSTEXPR constexpr
#else
#define RESTPP_NOEXCEPT
#define RESTPP_CONSTEXPR const
#endif // _MSC_VER >= 1900

#include <sal.h>

#else // ^^^ _WIN32 ^^^ // vvv !_WIN32 vvv

#define __declspec(x) __attribute__((x))
#define dllimport
#define novtable /* no novtable equivalent */
#define __assume(x)                                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(x)) __builtin_unreachable();                                                                             \
    } while (false)
#define RESTPP_NOEXCEPT noexcept
#define RESTPP_CONSTEXPR constexpr

#include <assert.h>
#define _ASSERTE(x) assert(x)

#if !defined(__cdecl)
#if defined(cdecl)
#define __cdecl __attribute__((cdecl))
#else // ^^^ defined cdecl ^^^ // vvv !defined cdecl vvv
#define __cdecl
#endif // defined cdecl
#endif // not defined __cdecl

#if defined(__ANDROID__)
// This is needed to disable the use of __thread inside the boost library.
// Android does not support thread local storage -- if boost is included
// without this macro defined, it will create references to __tls_get_addr
// which (while able to link) will not be available at runtime and prevent
// the .so from loading.
#if not defined BOOST_ASIO_DISABLE_THREAD_KEYWORD_EXTENSION
#define BOOST_ASIO_DISABLE_THREAD_KEYWORD_EXTENSION
#endif // not defined BOOST_ASIO_DISABLE_THREAD_KEYWORD_EXTENSION
#endif // defined(__ANDROID__)

#ifdef __clang__
#include <cstdio>
#endif // __clang__
#endif // _WIN32

#ifdef _NO_ASYNCRTIMP
#define _ASYNCRTIMP
#define _ASYNCRTIMP_TYPEINFO
#else // ^^^ _NO_ASYNCRTIMP ^^^ // vvv !_NO_ASYNCRTIMP vvv
#ifdef _ASYNCRT_EXPORT
#define _ASYNCRTIMP __declspec(dllexport)
#else // ^^^ _ASYNCRT_EXPORT ^^^ // vvv !_ASYNCRT_EXPORT vvv
#define _ASYNCRTIMP __declspec(dllimport)
#endif // _ASYNCRT_EXPORT

#if defined(_WIN32)
#define _ASYNCRTIMP_TYPEINFO
#else // ^^^ _WIN32 ^^^ // vvv !_WIN32 vvv
#define _ASYNCRTIMP_TYPEINFO __attribute__((visibility("default")))
#endif // _WIN32

#endif // _NO_ASYNCRTIMP

#ifdef RESTPP_DEPRECATION_NO_WARNINGS
#define RESTPP_DEPRECATED(x)
#else
#define RESTPP_DEPRECATED(x) __declspec(deprecated(x))
#endif // RESTPP_DEPRECATION_NO_WARNINGS

#endif // RESTPP_COMPAT_HPP
