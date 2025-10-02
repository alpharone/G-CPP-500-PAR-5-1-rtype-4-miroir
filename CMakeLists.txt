cmake_minimum_required(VERSION 3.16)
project(r-type LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_PLUGINS "Build dynamic plugins" ON)

include(CTest)

if (BUILD_TESTS)
    enable_testing()
endif()

include(cmake/CPM.cmake)

CPMAddPackage(
  NAME SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.6.x
)

CPMAddPackage(
  NAME Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.0
)

CPMAddPackage(
  NAME asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG asio-1-28-1 # choisis un tag stable
  OPTIONS "ASIO_SEPARATE_COMPILATION=OFF"
)

if(Catch2_ADDED)
  list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
  include(Catch)
endif()

add_subdirectory(project)

add_custom_target(fclean
    DEPENDS
      project_clean
      client_clean
      component_clean
      engine_clean
      ecs_clean
      game_clean
      network_clean
      server_clean
      system_clean
      tools_clean
    COMMENT "-> execute fclean"
    COMMAND ${CMAKE_COMMAND} -E remove
            "${CMAKE_SOURCE_DIR}/CMakeCache.txt"
    COMMAND ${CMAKE_COMMAND} -E remove
            "${CMAKE_SOURCE_DIR}/CPackConfig.cmake"
    COMMAND ${CMAKE_COMMAND} -E remove
            "${CMAKE_SOURCE_DIR}/CPackSourceConfig.cmake"
    COMMAND ${CMAKE_COMMAND} -E remove
            "${CMAKE_SOURCE_DIR}/cpm-package-lock.cmake"
    COMMAND ${CMAKE_COMMAND} -E remove
            "${CMAKE_SOURCE_DIR}/CTestTestfile.cmake"
    COMMAND ${CMAKE_COMMAND} -E remove
            "${CMAKE_SOURCE_DIR}/DartConfiguration.tcl"
    COMMAND ${CMAKE_COMMAND} -E remove
            "${CMAKE_SOURCE_DIR}/Makefile"
    COMMAND ${CMAKE_COMMAND} -E remove
            "${CMAKE_SOURCE_DIR}/cmake_install.cmake"
    COMMAND ${CMAKE_COMMAND} -E remove_directory
            "${CMAKE_SOURCE_DIR}/CMakeFiles"
    COMMAND ${CMAKE_COMMAND} -E remove_directory
            "${CMAKE_SOURCE_DIR}/Testing"
    COMMAND ${CMAKE_COMMAND} -E remove_directory
            "${CMAKE_SOURCE_DIR}/_deps"
    COMMAND ${CMAKE_COMMAND} -E remove_directory
            "${CMAKE_SOURCE_DIR}/plugins"
)

add_custom_target(re
    COMMENT "→ rebuild complet"
    COMMAND ${CMAKE_COMMAND} --build . --target fclean
    COMMAND cmake -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} --build .
)
