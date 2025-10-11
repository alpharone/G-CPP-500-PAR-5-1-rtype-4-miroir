add_library(ServerNetworkSystem SHARED
    src/ServerNetworkSystem.cpp
)

set_target_properties(ServerNetworkSystem PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/systems)

target_include_directories(ServerNetworkSystem
    PUBLIC
        include
        ${CMAKE_SOURCE_DIR}/project/engine/systems/include
        ${CMAKE_SOURCE_DIR}/project/engine/ecs/include
        ${CMAKE_SOURCE_DIR}/project/engine/components/include
        ${asio_SOURCE_DIR}/asio/include
)

target_link_libraries(ServerNetworkSystem
    PRIVATE
        ecs
        components
        network
        tools
        server_lib
)

if(BUILD_TESTS)
    add_executable(ServerNetworkSystemTest
        tests/ServerNetworkSystemTest.cpp
    )
    target_link_libraries(ServerNetworkSystemTest PRIVATE ServerNetworkSystem Catch2::Catch2WithMain)
    catch_discover_tests(ServerNetworkSystemTest)
endif()

add_custom_target(ServerNetworkSystem_clean
    COMMENT "-> start clean ServerNetworkSystem"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/plugins/systems/ServerlibNetworkSystem.so
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/ServerNetworkSystemTest*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> ServerNetworkSystem_clean finished"
)
