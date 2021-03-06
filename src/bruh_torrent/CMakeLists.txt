
cmake_minimum_required (VERSION 3.8)

set(bt_src
    "bruh_torrent.cpp"
    "torrent.cpp"
    "services/alert_service.cpp"
    "services/disk_io_service.cpp"
    "services/tracker_service.cpp"
    "services/torrent_file_parser.cpp"
    "services/tcp_service.cpp"
    "peer_connection/peer_connection.cpp"
    "peer_connection/bt_peer_connection.cpp"
    "utils/id_utils.cpp"
    "utils/file.cpp"
    "peer_messages.cpp"
    "services/disk_executor.cpp")
set(bt_headers
    "bruh_torrent.h"
    "peer_messages.h"
    "torrent.h"
    "base/core.h"
    "base/result.h"
    "base/types.h"
    "base/asserts.h"
    "services/alert_service.h"
    "services/disk_io_service.h"
    "services/torrent_file_parser.h"
    "services/tracker_service.h"
    "services/tcp_service.h"
    "peer_connection/peer_connection.h"
    "peer_connection/bt_peer_connection.h"
    "utils/endpoint.h"
    "utils/id_utils.h"
    "utils/file.h"
    "services/disk_executor.h")

if(${BT_PLATFORM} STREQUAL "Windows")
    add_definitions("-DBT_PLATFORM_WINDOWS")
endif()

if(${BT_BUILD_SHARED} OR CMAKE_BUILD_SHARED)
    add_library(BruhTorrent SHARED ${bt_src} ${bt_headers})
    add_definitions("-DBT_SHARED -DBT_EXPORT")
else()
    add_library(BruhTorrent STATIC ${bt_src} ${bt_headers})
endif()

target_include_directories(BruhTorrent PRIVATE "./")

target_precompile_headers(BruhTorrent PRIVATE "base/bt_pch.h")

if(${BT_DISABLE_ASSERTS})
    add_definitions("-DBT_DISABLE_ASSERTS")
endif()

# fmt:
find_package(fmt CONFIG REQUIRED)
target_link_libraries(BruhTorrent PRIVATE fmt::fmt)

# Boost context:
find_package(Boost 1.79.0 REQUIRED COMPONENTS context)
target_link_libraries(BruhTorrent PRIVATE Boost::boost Boost::context)
