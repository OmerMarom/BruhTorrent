
add_subdirectory("bruh_torrent")
if (${BT_ENABLE_TESTS})
    message(STATUS "Generating tests.")
    add_subdirectory("tests")
endif()
