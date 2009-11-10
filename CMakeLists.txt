project(compiler)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_FLAGS, "-O2 -Wall -pedantic -g0")
set(CMAKE_CXX_FLAGS, "-O2 -Wall -pedantic -g0")

set(CMAKE_C_FLAGS_DEBUG, "-O0 -g3 -Wall -pedantic -pg")
set(CMAKE_CXX_FLAGS_DEBUG, "-O0 -g3 -Wall -pedantic -pg")

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_subdirectory(lib)
add_subdirectory(src)
