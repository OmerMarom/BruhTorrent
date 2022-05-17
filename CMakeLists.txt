
cmake_minimum_required(VERSION 3.8)

project("BruhTorrent")

# C++ Standards:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# Project options:
option(BT_DISABLE_ASSERTS "Disable asserts." OFF)
option(BT_ENABLE_TESTS "Enable tests." OFF)
option(BT_BUILD_SHARED "Build BruhTorrent as shared library. (Same as CMAKE_BUILD_SHARED)" ON)
set(BT_PLATFORM ${CMAKE_HOST_SYSTEM_NAME} CACHE STRING "BruhTorrent target platform (currently, only [Windows] is available.")

add_subdirectory("src")
