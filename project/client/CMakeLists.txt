add_library(client_lib
    src/Client.cpp
)

target_include_directories(client_lib PUBLIC include)
target_link_libraries(client_lib
    PRIVATE
        engine
        network
        game
        tools
)

add_executable(r-type_client
    src/main.cpp
)

set_target_properties(r-type_client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

target_link_libraries(r-type_client
    PRIVATE
        client_lib
        sfml-graphics sfml-audio sfml-window sfml-system
)

if(BUILD_TESTS)
    add_executable(client_tests
        tests/ClientTest.cpp
    )
    target_link_libraries(client_tests PRIVATE client_lib Catch2::Catch2WithMain)
    catch_discover_tests(client_tests)
endif()

add_custom_target(client_clean
    COMMENT "-> start clean client"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/r-type_client
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/client_tests
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/client_tests*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/libclient_lib.so
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> client_clean finished"
)
