
cmake_minimum_required(VERSION 3.8)

project("BruhTorrent")

# C++ Standards:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# Project options:
option(BT_DISABLE_ASSERTS "Disable asserts." OFF)
option(BT_ENABLE_TESTS "Enable tests." OFF)

add_subdirectory("src")
