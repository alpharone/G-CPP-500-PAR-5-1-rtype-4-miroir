add_library(CleanupSystem SHARED
    src/CleanupSystem.cpp
)

set_target_properties(CleanupSystem PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/systems)

target_include_directories(CleanupSystem
    PUBLIC
        include
        ${CMAKE_SOURCE_DIR}/project/engine/systems/include
        ${CMAKE_SOURCE_DIR}/project/engine/ecs/include
        ${CMAKE_SOURCE_DIR}/project/engine/components/include
)

target_link_libraries(CleanupSystem
    PRIVATE
        ecs
        components
        tools
)

if(BUILD_TESTS)
    add_executable(CleanupSystemTest
        tests/CleanupSystemTest.cpp
    )
    target_link_libraries(CleanupSystemTest PRIVATE CleanupSystem Catch2::Catch2WithMain)
    catch_discover_tests(CleanupSystemTest)
endif()

add_custom_target(CleanupSystem_clean
    COMMENT "-> start clean CleanupSystem"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/plugins/systems/libCleanupSystem.so
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CleanupSystemTest*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> CleanupSystem_clean finished"
)
