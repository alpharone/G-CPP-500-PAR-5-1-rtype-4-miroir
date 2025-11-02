add_library(game STATIC
    src/GameRunner.cpp
)

target_include_directories(game PUBLIC include ${nlohmann_json_SOURCE_DIR}/include)

target_link_libraries(game
    PUBLIC
        nlohmann_json::nlohmann_json
        ecs
        network
        systems
        tools
        components
        ClientNetworkSystem
)

add_library(shared::game ALIAS game)

if(BUILD_TESTS)
    add_executable(game_tests
        tests/GameRunnerTest.cpp
    )
    target_link_libraries(game_tests PRIVATE game Catch2::Catch2WithMain)
    catch_discover_tests(game_tests)
endif()

add_custom_target(game_clean
    COMMENT "-> start clean game"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/game_tests*
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/libgame.*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> game_clean finished"
)
