add_library(engine
    src/Entity.cpp
    src/Registry.cpp
)

target_include_directories(engine PUBLIC include)

if(BUILD_TESTS)
    add_executable(engine_tests
        tests/EntityTest.cpp
        tests/RegistryTest.cpp
    )
    target_link_libraries(engine_tests PRIVATE engine Catch2::Catch2WithMain)
    catch_discover_tests(engine_tests)
endif()

add_library(shared::engine ALIAS engine)

add_custom_target(engine_clean
    COMMENT "-> start clean engine"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/engine_tests*
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/libengine.so
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> engine_clean finished"
)
