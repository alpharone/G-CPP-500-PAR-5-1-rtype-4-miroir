add_library(components INTERFACE)

target_include_directories(components INTERFACE include)

target_link_libraries(components INTERFACE raylib)

add_library(shared::components ALIAS components)

add_custom_target(component_clean
    COMMENT "-> start clean component"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> component_clean finished"
)
