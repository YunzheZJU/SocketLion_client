cmake_minimum_required(VERSION 3.9)
project(SocketLion_client)

set(CMAKE_CXX_STANDARD 11)

add_executable(SocketLion_client main.cpp)

# Linking
target_link_libraries(SocketLion_client ws2_32.lib)