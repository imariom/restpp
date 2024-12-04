function(restpp_find_openssl)
    if(TARGET restpp_openssl_internal)
        return()
    endif()

    if(APPLE)
      if(NOT DEFINED OPENSSL_ROOT_DIR)
        # Prefer a homebrew version of OpenSSL over the one in /usr/lib
        file(GLOB OPENSSL_ROOT_DIR /usr/local/Cellar/openssl*/*)
        # Prefer the latest (make the latest one first)
        list(REVERSE OPENSSL_ROOT_DIR)
        list(GET OPENSSL_ROOT_DIR 0 OPENSSL_ROOT_DIR)
      endif()
      # This should prevent linking against the system provided 0.9.8y
      message(STATUS "OPENSSL_ROOT_DIR = ${OPENSSL_ROOT_DIR}")
      set(_OPENSSL_VERSION "")
    endif()
    if(UNIX)
      find_package(PkgConfig)
      pkg_search_module(OPENSSL openssl)
    endif()
    if(OPENSSL_FOUND)
      target_link_libraries(restpp PRIVATE ${OPENSSL_LDFLAGS})
    else()
      find_package(OpenSSL 1.0.0 REQUIRED)
    endif()

    INCLUDE(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_INCLUDES "${OPENSSL_INCLUDE_DIR}")
    set(CMAKE_REQUIRED_LIBRARIES "${OPENSSL_LIBRARIES}")
    CHECK_CXX_SOURCE_COMPILES("
      #include <openssl/ssl.h>
      int main()
      {
      ::SSL_COMP_free_compression_methods();
      }
    " _SSL_LEAK_SUPPRESS_AVAILABLE)

    add_library(restpp_openssl_internal INTERFACE)
    if(TARGET OpenSSL::SSL)
        target_link_libraries(restpp_openssl_internal INTERFACE OpenSSL::SSL)
    else()
        target_link_libraries(restpp_openssl_internal INTERFACE "$<BUILD_INTERFACE:${OPENSSL_LIBRARIES}>")
        target_include_directories(restpp_openssl_internal INTERFACE "$<BUILD_INTERFACE:${OPENSSL_INCLUDE_DIR}>")
    endif()

    if (NOT _SSL_LEAK_SUPPRESS_AVAILABLE)
        # libressl doesn't ship with the cleanup method being used in ws_client_wspp
        target_compile_definitions(restpp_openssl_internal INTERFACE -DCPPREST_NO_SSL_LEAK_SUPPRESS)
    endif()
endfunction()
