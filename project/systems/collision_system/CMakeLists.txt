add_library(CollisionSystem SHARED
    src/CollisionSystem.cpp
)

set_target_properties(CollisionSystem PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/systems)

target_include_directories(CollisionSystem
    PUBLIC
        include
        ${CMAKE_SOURCE_DIR}/project/systems/include
        ${CMAKE_SOURCE_DIR}/project/engine/include
        ${CMAKE_SOURCE_DIR}/project/components/include
)

target_link_libraries(CollisionSystem
    PRIVATE
        engine
        components
        tools
)

if(BUILD_TESTS)
    add_executable(CollisionSystemTest
        tests/CollisionSystemTest.cpp
    )
    target_link_libraries(CollisionSystemTest PRIVATE CollisionSystem Catch2::Catch2WithMain)
    catch_discover_tests(CollisionSystemTest)
endif()

add_custom_target(CollisionSystem_clean
    COMMENT "-> start clean CollisionSystem"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/plugins/systems/libCollisionSystem.so
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CollisionSystemTest*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> CollisionSystem_clean finished"
)
