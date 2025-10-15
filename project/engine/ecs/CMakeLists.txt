add_library(ecs
    src/Entity.cpp
    src/Registry.cpp
)

target_include_directories(ecs PUBLIC include)

if(BUILD_TESTS)
    add_executable(ecs_tests
        tests/EntityTest.cpp
        tests/RegistryTest.cpp
    )
    target_link_libraries(ecs_tests PRIVATE ecs Catch2::Catch2WithMain)
    catch_discover_tests(ecs_tests)
endif()

add_library(shared::ecs ALIAS ecs)

add_custom_target(ecs_clean
    COMMENT "-> start clean ecs"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/ecs_tests*
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/engine_tests-b12d07c_include.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/libecs.*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> ecs_clean finished"
)
