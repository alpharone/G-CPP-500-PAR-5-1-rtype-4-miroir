add_library(RenderSystem SHARED
    src/RenderSystem.cpp
)

set_target_properties(RenderSystem PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/systems)

target_include_directories(RenderSystem
    PUBLIC
        include
        ${CMAKE_SOURCE_DIR}/project/engine/systems/include
        ${CMAKE_SOURCE_DIR}/project/engine/ecs/include
        ${CMAKE_SOURCE_DIR}/project/engine/components/include
)

target_link_libraries(RenderSystem
    PRIVATE
        ecs
        components
        tools
)

if(BUILD_TESTS)
    add_executable(RenderSystemTest
        tests/RenderSystemTest.cpp
    )
    target_link_libraries(RenderSystemTest PRIVATE RenderSystem Catch2::Catch2WithMain)
    catch_discover_tests(RenderSystemTest)
endif()

add_custom_target(RenderSystem_clean
    COMMENT "-> start clean RenderSystem"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/plugins/systems/libRenderSystem.so
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/RenderSystemTest*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> RenderSystem_clean finished"
)
