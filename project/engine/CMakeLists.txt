add_subdirectory(components)
add_subdirectory(ecs)
add_subdirectory(game)
add_subdirectory(network)
add_subdirectory(systems)
add_subdirectory(tools)

add_custom_target(engine_clean
    COMMENT "-> start clean engine"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> engine_clean finished"
)