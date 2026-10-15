
cmake_minimum_required(VERSION 3.20)
project(udp_sandbox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_executable(udp_sandbox
    src/main.cpp
    src/net/udp_client.cpp
    src/game/player.cpp
    src/game/renderer.cpp
)

target_include_directories(udp_sandbox PRIVATE src)
target_link_libraries(udp_sandbox PRIVATE raylib)
