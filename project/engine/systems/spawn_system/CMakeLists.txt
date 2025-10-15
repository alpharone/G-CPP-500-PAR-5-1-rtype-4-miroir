add_library(SpawnSystem SHARED
    src/SpawnSystem.cpp
)

set_target_properties(SpawnSystem PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/systems)

target_include_directories(SpawnSystem
    PUBLIC
        include
        ${CMAKE_SOURCE_DIR}/project/engine/systems/include
        ${CMAKE_SOURCE_DIR}/project/engine/ecs/include
        ${CMAKE_SOURCE_DIR}/project/engine/components/include
)

target_link_libraries(SpawnSystem
    PRIVATE
        ecs
        components
        tools
)

if(BUILD_TESTS)
    add_executable(SpawnSystemTest
        tests/SpawnSystemTest.cpp
    )
    target_link_libraries(SpawnSystemTest PRIVATE SpawnSystem Catch2::Catch2WithMain)
    catch_discover_tests(SpawnSystemTest)
endif()

add_custom_target(SpawnSystem_clean
    COMMENT "-> start clean SpawnSystem"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/plugins/systems/libSpawnSystem.so
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/SpawnSystemTest*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> SpawnSystem_clean finished"
)
