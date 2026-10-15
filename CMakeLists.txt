
cmake_minimum_required(VERSION 3.20)
project(udp-sandbox-client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_executable(udp-sandbox-client
    src/main.cpp
    src/net/udp_client.cpp
    src/game/player.cpp
    src/game/renderer.cpp
)

target_include_directories(udp-sandbox-client PRIVATE src)
target_link_libraries(udp-sandbox-client PRIVATE raylib)
