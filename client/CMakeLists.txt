cmake_minimum_required(VERSION 3.5)

set(CLIENT_NAME client)

add_executable(${CLIENT_NAME})
target_sources(${CLIENT_NAME} PRIVATE client.c)
target_compile_options(${CLIENT_NAME} PRIVATE "-O2;-Wall;-Wextra;-Wpedantic;")
target_include_directories(${CLIENT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

