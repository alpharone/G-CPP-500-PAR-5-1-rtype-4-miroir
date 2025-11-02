set(TARGET AnimationSystem)

add_library(${TARGET} SHARED
    src/AnimationSystem.cpp
)

set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/systems)

target_include_directories(${TARGET}
    PUBLIC
        include
        ${CMAKE_SOURCE_DIR}/project/engine/systems/include
        ${CMAKE_SOURCE_DIR}/project/engine/ecs/include
        ${CMAKE_SOURCE_DIR}/project/engine/components/include
)

target_link_libraries(${TARGET}
    PRIVATE
        ecs
        components
        tools
        raylib
        network
)

if(BUILD_TESTS)
    add_executable(AnimationSystemTest
        tests/AnimationSystemTest.cpp
    )
    target_link_libraries(AnimationSystemTest PRIVATE ${TARGET} Catch2::Catch2WithMain)
    catch_discover_tests(AnimationSystemTest)
endif()

add_custom_target(AnimationSystem_clean
    COMMENT "-> start clean AnimationSystem"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/plugins/systems/libAnimationSystem.so
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/AnimationSystemTest*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> AnimationSystem_clean finished"
)
