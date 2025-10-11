add_library(ClientNetworkSystem SHARED
    src/ClientNetworkSystem.cpp
)

set_target_properties(ClientNetworkSystem PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/systems)

target_include_directories(ClientNetworkSystem
    PUBLIC
        include
        ${CMAKE_SOURCE_DIR}/project/engine/systems/include
        ${CMAKE_SOURCE_DIR}/project/engine/ecs/include
        ${CMAKE_SOURCE_DIR}/project/engine/components/include
        ${asio_SOURCE_DIR}/asio/include
)

target_link_libraries(ClientNetworkSystem
    PRIVATE
        ecs
        network
        components
        tools
        raylib
)

if(BUILD_TESTS)
    add_executable(ClientNetworkSystemTest
        tests/ClientNetworkSystemTest.cpp
    )
    target_link_libraries(ClientNetworkSystemTest PRIVATE ClientNetworkSystem Catch2::Catch2WithMain)
    catch_discover_tests(ClientNetworkSystemTest)
endif()

add_custom_target(ClientNetworkSystem_clean
    COMMENT "-> start clean ClientNetworkSystem"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/plugins/systems/libClientNetworkSystem.so
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/ClientNetworkSystemTest*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> ClientNetworkSystem_clean finished"
)
