cmake_minimum_required(VERSION 3.5)

set(SERVER_NAME server)

add_executable(${SERVER_NAME})
target_sources(${SERVER_NAME} PRIVATE server.c)
target_compile_options(${SERVER_NAME} PRIVATE "-O2;-Wall;-Wextra;-Wpedantic;")
target_include_directories(${SERVER_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
