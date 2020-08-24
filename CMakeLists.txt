cmake_minimum_required(VERSION 3.5)

project(Linux-network LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(${CMAKE_SOURCE_DIR}/client)
add_subdirectory(${CMAKE_SOURCE_DIR}/server)
