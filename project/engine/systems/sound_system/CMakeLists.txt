set(TARGET SoundSystem)

add_library(${TARGET} SHARED
    src/SoundSystem.cpp
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
)

if(BUILD_TESTS)
    add_executable(SoundSystemTest
        tests/SoundSystemTest.cpp
    )
    target_link_libraries(SoundSystemTest PRIVATE ${TARGET} Catch2::Catch2WithMain)
    catch_discover_tests(SoundSystemTest)
endif()

add_custom_target(SoundSystem_clean
    COMMENT "-> start clean SoundSystem"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/plugins/systems/libSoundSystem.so
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/SoundSystemTest*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> SoundSystem_clean finished"
)
