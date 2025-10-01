add_library(tools
    src/Logger.cpp
    src/ConfigManager.cpp
)

target_include_directories(tools PUBLIC include)

if(BUILD_TESTS)
    add_executable(tools_tests
        tests/LoggerTest.cpp
        tests/ConfigManagerTest.cpp
    )
    target_link_libraries(tools_tests PRIVATE tools Catch2::Catch2WithMain)
    catch_discover_tests(tools_tests)
endif()

add_custom_target(tools_clean
    COMMENT "-> start clean tools"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/libtools.so
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/tools_tests*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> tools_clean finished"
)
