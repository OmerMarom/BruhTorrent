
cmake_minimum_required (VERSION 3.8)

set(bt_src
    "bruh_torrent.cpp"
    "torrent.cpp"
    "peer.cpp"
    "services/alert_service.cpp"
    "services/disk_io_service.cpp"
    "services/tracker_service.cpp"
)
set(bt_headers
    "bruh_torrent.h"
    "peer.h"
    "torrent.h"
    "services/alert_service.h"
    "services/disk_io_service.h"
    "services/torrent_file_parser.h"
    "services/tracker_service.h"
    "utils/asserts.h"
    "utils/endpoint.h"
    "utils/id_utils.h"
    "utils/result.h"
    "utils/types.h"
)

add_library(BruhTorrent SHARED ${bt_src} ${bt_headers})

target_include_directories(BruhTorrent PRIVATE "./")

target_precompile_headers(BruhTorrent PRIVATE "bt_pch.h")

if(${BT_DISABLE_ASSERTS})
    add_definitions("-DBT_DISABLE_ASSERTS")
endif()
