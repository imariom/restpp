cmake_minimum_required(VERSION 3.9)

if(UNIX)
    project(restpp C CXX)
else()
    project(restpp CXX)
endif()

set(RESTPP_VERSION_MAJOR 1)
set(RESTPP_VERSION_MINOR 0)
set(RESTPP_VERSION_REVISION 0)

enable_testing()

set(WERROR ON CACHSE BOOL "Treat Warnings as Errors.")
set(RESTPP_EXCLUDE_FRAMEWORK OFF CACHE BOOL "Exclude restpp RESTful framework functionality.")
set(RESTPP_EXPORT_DIR cmake/restpp CACHE STRING "Directory to install CMake config files.")
set(RESTPP_INSTALL_HEADERS ON CACHE BOOL "Install header files.")
set(RESTPP_INSTALL ON CACHE BOOL "Add install commands.")
set(BUILD_SAMPLES ON)

include(cmake/restpp_find_boost.cmake)
include(cmake/restpp_find_openssl.cmake)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(BUILD_SAMPLES)
  add_subdirectory(example)
endif()
