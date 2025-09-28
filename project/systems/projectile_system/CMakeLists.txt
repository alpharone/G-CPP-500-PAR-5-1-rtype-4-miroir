add_library(ProjectileSystem SHARED
    src/ProjectileSystem.cpp
)

set_target_properties(ProjectileSystem PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/systems)

target_include_directories(ProjectileSystem
    PUBLIC
        include
        ${CMAKE_SOURCE_DIR}/project/systems/include
        ${CMAKE_SOURCE_DIR}/project/engine/include
        ${CMAKE_SOURCE_DIR}/project/components/include
)

target_link_libraries(ProjectileSystem
    PRIVATE
        engine
        components
        tools
)

if(BUILD_TESTS)
    add_executable(ProjectileSystemTest
        tests/ProjectileSystemTest.cpp
    )
    target_link_libraries(ProjectileSystemTest PRIVATE ProjectileSystem Catch2::Catch2WithMain)
    catch_discover_tests(ProjectileSystemTest)
endif()

add_custom_target(ProjectileSystem_clean
    COMMENT "-> start clean ProjectileSystem"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/plugins/systems/libProjectileSystem.so
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/ProjectileSystemTest*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> ProjectileSystem_clean finished"
)
