add_library(systems INTERFACE)
target_include_directories(systems INTERFACE include)

add_library(shared::systems ALIAS systems)

add_subdirectory(position_system)
add_subdirectory(control_system)
add_subdirectory(collision_system)
add_subdirectory(physics_system)
add_subdirectory(ai_system)
add_subdirectory(projectile_system)
add_subdirectory(render_system)
add_subdirectory(sound_system)
add_subdirectory(cleanup_system)
add_subdirectory(spawn_system)

add_custom_target(system_clean
    DEPENDS
        AISystem_clean
        CleanupSystem_clean
        CollisionSystem_clean
        ControlSystem_clean
        PhysicsSystem_clean
        PositionSystem_clean
        ProjectileSystem_clean
        RenderSystem_clean
        SoundSystem_clean
        SpawnSystem_clean
    COMMENT "-> start clean system"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove_directory -f ${CMAKE_SOURCE_DIR}/plugins/systems
    COMMENT "=> system_clean finished"
)
