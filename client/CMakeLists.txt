cmake_minimum_required(VERSION 3.5)

set(CLIENT_NAME client)

add_executable(${CLIENT_NAME})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${CLIENT_NAME} Threads::Threads)

target_sources(${CLIENT_NAME} PRIVATE client.c)
target_compile_options(${CLIENT_NAME} PRIVATE "-O2;-Wall;-Wextra;-Wpedantic;-pthread")
target_include_directories(${CLIENT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

