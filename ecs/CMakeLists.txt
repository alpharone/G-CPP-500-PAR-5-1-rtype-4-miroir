cmake_minimum_required(VERSION 3.16)
project(R-type LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.6.x
)

FetchContent_MakeAvailable(SFML)

set(SOURCE_FILES
    ./src/main.cpp
    ./src/Entity.cpp
    ./src/Registry.cpp
    ./src/System.cpp
)

set(INCLUDE_FILES
    ./include
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(r-type ${SOURCE_FILES})

target_link_libraries(r-type PRIVATE sfml-system sfml-window sfml-graphics sfml-audio)

target_include_directories(r-type PRIVATE ${INCLUDE_FILES})

add_custom_target(r-type_clean
    COMMENT "-> start clean r-type"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/r-type
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> r-type_clean finished"
)

add_custom_target(r-type_re
    DEPENDS r-type_clean r-type
    COMMENT "-> start rebuild r-type"
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR} --target r-type
    COMMENT "=> rebuild r-type finished"
)

add_custom_target(fclean
    COMMENT "-> fclean process starting !"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_SOURCE_DIR}/CPackSourceConfig.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_SOURCE_DIR}/CPackConfig.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/_deps
    COMMENT "=> fclean process finished"
)
