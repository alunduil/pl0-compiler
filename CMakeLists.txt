PROJECT(brandtal.proj1)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug)
include_directories(include)

ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(src)
