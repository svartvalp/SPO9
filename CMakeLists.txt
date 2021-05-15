cmake_minimum_required(VERSION 3.17)
project(prak9)

set(CMAKE_CXX_STANDARD 14)

add_executable(prak9_server server/main.cpp)
add_executable(prak9_client client/main.cpp)