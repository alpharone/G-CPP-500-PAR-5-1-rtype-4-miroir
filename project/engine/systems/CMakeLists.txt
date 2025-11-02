add_library(systems
    src/SystemManager.cpp
    src/SystemCatalog.cpp
)

target_include_directories(systems 
    PUBLIC
        include
)

target_link_libraries(systems
    PUBLIC
        ecs
        tools
)

add_library(shared::systems ALIAS systems)

add_subdirectory(position_system)
add_subdirectory(collision_system)
add_subdirectory(physics_system)
add_subdirectory(ai_system)
add_subdirectory(projectile_system)
add_subdirectory(render_system)
add_subdirectory(server_network_system)
add_subdirectory(client_network_system)
add_subdirectory(sound_system)
add_subdirectory(input_system)
add_subdirectory(animation_system)

add_custom_target(system_clean
    DEPENDS
        AISystem_clean
        CollisionSystem_clean
        PhysicsSystem_clean
        PositionSystem_clean
        ProjectileSystem_clean
        ServerNetworkSystem_clean
        ClientNetworkSystem_clean
        RenderSystem_clean
        SoundSystem_clean
        InputSystem_clean
        AnimationSystem_clean
    COMMENT "-> start clean system"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/libsystems.*
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove_directory -f ${CMAKE_SOURCE_DIR}/plugins/systems
    COMMENT "=> system_clean finished"
)
