set(SOURCES
    main.cpp)

add_executable(restpp ${SOURCES})

restpp_find_boost()
target_link_libraries(restpp PUBLIC restpp_boost_internal)
target_include_directories(restpp PRIVATE ${CMAKE_SOURCE_DIR}/include)
