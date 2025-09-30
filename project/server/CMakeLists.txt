add_library(server_lib
    src/Server.cpp
    src/ReliableChannel.cpp
)

target_include_directories(server_lib PUBLIC include ${asio_SOURCE_DIR}/asio/include)
target_link_libraries(server_lib
    PUBLIC
        engine
        network
        game
        tools
)

add_executable(r-type_server
    src/main.cpp
    src/Server.cpp
    src/LobbyManager.cpp
)

set_target_properties(r-type_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

target_link_libraries(r-type_server
    PRIVATE
        server_lib
)

if(BUILD_TESTS)
    add_executable(server_tests
        tests/ServerTest.cpp
        tests/ReliableChannelTest.cpp
        tests/LobbyManagerTest.cpp
    )
    target_link_libraries(server_tests PRIVATE server_lib Catch2::Catch2WithMain)
    catch_discover_tests(server_tests)
endif()

add_custom_target(server_clean
    COMMENT "-> start clean server"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/r-type_server
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/server_tests
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/server_tests*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/libserver_lib.so
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> server_clean finished"
)
