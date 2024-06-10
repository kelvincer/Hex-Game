cmake_minimum_required(VERSION 3.28)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

find_package(fmt CONFIG REQUIRED)

add_executable(untitled main.cpp)

target_link_libraries(untitled PRIVATE fmt::fmt)
