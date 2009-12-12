project(compiler)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE Release)

if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DNDEBUG)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_subdirectory(lib)
add_subdirectory(src)
