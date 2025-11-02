# Adding Systems

This guide explains how to implement and integrate a new system into the R-Type engine.

## Overview

Systems in this ECS architecture are responsible for updating entities based on their components. All systems must inherit from `ISystem` and be loaded dynamically as plugins.

## Requirement

Tree structure of a system to be placed in `project/engine/systems/`:

```bash
your_system/
├── CMakeLists.txt
├── include/
    └── YourSystem.hpp
├── src/
    └── YourSystem.cpp
└── tests/
    └── YourSystemTest.cpp
```

## Step 1: Define the System Header

Create `project/engine/systems/your_system/include/YourSystem.hpp`:

```cpp
#pragma once

#include "ISystem.hpp"
#include "Registry.hpp"

namespace System {

class YourSystem : public ISystem {
    public:
    YourSystem();  // Constructor, can take config params
    ~YourSystem() override = default;

    void init(Ecs::Registry &registry) override;
    void update(Ecs::Registry &registry, double dt) override;
    void shutdown() override;

    private:
    // Add private members as needed
};

} // namespace System
```

## Step 2: Implement the System

Create `project/engine/systems/your_system/src/YourSystem.cpp`:

```cpp
#include "YourSystem.hpp"

namespace System {

YourSystem::YourSystem() = default;

void YourSystem::init(Ecs::Registry &registry) {
    // Initialization logic, if needed
}

void YourSystem::update(Ecs::Registry &registry, double dt) {
    // Main update logic
    // Use registry.getComponents<Component>() to access relevant components
    // Iterate over entities and update them
}

void YourSystem::shutdown() {
    // Cleanup, if needed
}

} // namespace System

// Required extern "C" for plugin loading
extern "C" std::shared_ptr<System::ISystem>
createYourSystem(std::any params) {
    try {
        // Parse params if needed, e.g., config
        return std::make_shared<System::YourSystem>();
    } catch (const std::exception &e) {
        Logger::error(std::string("[Factory]: Failed to create YourSystem: ") + e.what());
        return nullptr;
    }
}
```

## Step 3: Add CMake Configuration

Create `project/engine/systems/your_system/CMakeLists.txt`:

```cmake
set(TARGET YourSystem)

add_library(${TARGET} SHARED
    src/YourSystem.cpp
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
        network
)

if(BUILD_TESTS)
    add_executable(YourSystemTest
        tests/YourSystemTest.cpp
    )
    target_link_libraries(YourSystemTest PRIVATE ${TARGET} Catch2::Catch2WithMain)
    catch_discover_tests(YourSystemTest)
endif()

add_custom_target(YourSystem_clean
    COMMENT "-> start clean YourSystem"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/plugins/systems/libYourSystem.so
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/YourSystemTest*
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
    COMMENT "=> YourSystem_clean finished"
)
```

Add to `project/engine/systems/CMakeLists.txt`:

```cmake
add_subdirectory(YourSystem)
```

## Step 4: Update Configuration

Add to `config/server.json` or `config/client.json`:

```json
{
  "name": "YourSystem",
  "lib": "./plugins/systems/libYourSystem.dylib",
  "factory": "createYourSystem",
  "params": []
}
```

## Example: Simple Movement System

```cpp
void MovementSystem::update(Ecs::Registry &registry, double dt) {
    auto &positions = registry.getComponents<Component::position_t>();
    auto &velocities = registry.getComponents<Component::velocity_t>();

    for (size_t i = 0; i < positions.size(); i++) {
        if (positions[i].has_value() && velocities[i].has_value()) {
            positions[i]->x += velocities[i]->vx * dt;
            positions[i]->y += velocities[i]->vy * dt;
        }
    }
}
```

## Notes

- Systems are called with a timestep `dt` for frame-rate independent updates.
- Use `std::optional` checks since components may not be on every entity.
- For server-side systems, logic should be deterministic and independent of render.
- Test the system by building and running it in the context of the full engine.
