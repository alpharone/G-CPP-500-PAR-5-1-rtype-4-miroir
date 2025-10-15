add_library(PhysicsSystem SHARED
    src/PhysicsSystem.cpp
)

set_target_properties(PhysicsSystem PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/systems)

target_include_directories(PhysicsSystem
    PUBLIC
        include
        ${CMAKE_SOURCE_DIR}/project/engine/systems/include
        ${CMAKE_SOURCE_DIR}/project/engine/ecs/include
        ${CMAKE_SOURCE_DIR}/project/engine/components/include
)

target_link_libraries(PhysicsSystem
    PRIVATE
        ecs
        components
        tools
)

if(BUILD_TESTS)
    add_executable(PhysicsSystemTest
        tests/PhysicsSystemTest.cpp
    )
    target_link_libraries(PhysicsSystemTest PRIVATE PhysicsSystem Catch2::Catch2WithMain)
    catch_discover_tests(PhysicsSystemTest)
endif()

add_custom_target(PhysicsSystem_clean
    COMMENT "-> start clean PhysicsSystem"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/plugins/systems/libPhysicsSystem.so
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/PhysicsSystemTest*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> PhysicsSystem_clean finished"
)
