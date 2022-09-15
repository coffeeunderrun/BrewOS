cmake_minimum_required(VERSION 3.21)

include(toolchain.cmake)

project(brewos)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(kernel)
