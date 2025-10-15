add_library(PositionSystem SHARED
    src/PositionSystem.cpp
)

set_target_properties(PositionSystem PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/systems)

target_include_directories(PositionSystem
    PUBLIC
        include
        ${CMAKE_SOURCE_DIR}/project/engine/systems/include
        ${CMAKE_SOURCE_DIR}/project/engine/ecs/include
        ${CMAKE_SOURCE_DIR}/project/engine/components/include
)

target_link_libraries(PositionSystem
    PRIVATE
        ecs
        components
        tools
)

if(BUILD_TESTS)
    add_executable(PositionSystemTest
        tests/PositionSystemTest.cpp
    )
    target_link_libraries(PositionSystemTest PRIVATE PositionSystem Catch2::Catch2WithMain)
    catch_discover_tests(PositionSystemTest)
endif()

add_custom_target(PositionSystem_clean
    COMMENT "-> start clean PositionSystem"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/plugins/systems/libPositionSystem.so
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/PositionSystemTest*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> PositionSystem_clean finished"
)
