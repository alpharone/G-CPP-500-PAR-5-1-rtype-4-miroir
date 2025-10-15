add_library(network
    src/Packets.cpp
    src/ReliableChannel.cpp
    src/Utils.cpp
)

target_include_directories(network PUBLIC include ${asio_SOURCE_DIR}/asio/include)
target_link_libraries(network 
    PRIVATE
        ecs
)

if(BUILD_TESTS)
    add_executable(network_tests
        tests/PacketTest.cpp
    )
    target_link_libraries(network_tests PRIVATE network Catch2::Catch2WithMain)
    catch_discover_tests(network_tests)
endif()

add_custom_target(network_clean
    COMMENT "-> start clean network"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/network_tests*
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/libnetwork.*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> network_clean finished"
)
