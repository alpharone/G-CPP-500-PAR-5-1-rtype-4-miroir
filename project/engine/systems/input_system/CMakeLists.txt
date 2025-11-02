set(TARGET InputSystem)

add_library(${TARGET} SHARED
    src/InputSystem.cpp
)

set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/systems)

target_include_directories(${TARGET}
    PUBLIC
        include
        ${CMAKE_SOURCE_DIR}/project/engine/systems/include
        ${CMAKE_SOURCE_DIR}/project/engine/ecs/include
        ${CMAKE_SOURCE_DIR}/project/engine/components/include
        ${CMAKE_SOURCE_DIR}/libs/raylib/include
        ${asio_SOURCE_DIR}/asio/include
)

target_link_libraries(${TARGET}
    PRIVATE
        ecs
        components
        tools
        network
        raylib
)

if(BUILD_TESTS)
    add_executable(InputSystemTest
        tests/InputSystemTest.cpp
    )
    target_link_libraries(InputSystemTest PRIVATE ${TARGET} Catch2::Catch2WithMain)
    catch_discover_tests(InputSystemTest)
endif()

add_custom_target(InputSystem_clean
    COMMENT "-> start clean InputSystem"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/plugins/systems/libInputSystem.so
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/InputSystemTest*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> InputSystem_clean finished"
)
