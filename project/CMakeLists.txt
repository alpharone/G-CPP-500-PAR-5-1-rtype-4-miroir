add_subdirectory(engine)
add_subdirectory(components)
add_subdirectory(systems)
add_subdirectory(server)
add_subdirectory(game)
add_subdirectory(client)
add_subdirectory(network)
add_subdirectory(tools)

add_custom_target(project_clean
    COMMENT "-> start clean project"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> project_clean finished"
)