function(restpp_find_boost)
    if(TARGET restpp_boost_internal)
        return()
    endif()

    find_package(Boost REQUIRED system thread regex)
    add_library(restpp_boost_internal INTERFACE)
    # FindBoost continually breaks imported targets whenever boost updates.
    # This hack was extracted from cpprestsdk library.
    if(1)
        target_include_directories(restpp_boost_internal INTERFACE "$<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>")
        set(_prev)
        set(_libs)
        foreach(_lib ${Boost_LIBRARIES})
            if(_lib STREQUAL "optimized" OR _lib STREQUAL "debug")
            else()
                if(_prev STREQUAL "optimized")
                  list(APPEND _libs "$<$<NOT:$<CONFIG:Debug>>:${_lib}>")
                elseif(_prev STREQUAL "debug")
                  list(APPEND _libs "$<$<CONFIG:Debug>:${_lib}>")
                else()
                  list(APPEND _libs "${_lib}")
                endif()
            endif()
            set(_prev "${_lib}")
        endforeach()
        target_link_libraries(restpp_boost_internal INTERFACE "$<BUILD_INTERFACE:${_libs}>")
    else()
        if(UNIX)
          target_link_libraries(restpp_boost_internal INTERFACE
            Boost::boost
            Boost::system
            Boost::thread
            Boost::regex
          )
        else()
          target_link_libraries(restpp_boost_internal INTERFACE
            Boost::boost
            Boost::system
            Boost::regex
          )
        endif()
    endif()
endfunction()
