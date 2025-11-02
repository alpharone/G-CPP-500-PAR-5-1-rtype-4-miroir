# ECS Usage

This guide explains how to use the Entity-Component-System architecture in the project.

## Entities

Entities are IDs (size_t) representing objects in the world.

```cpp
Ecs::Entity e = registry.spawnEntity();
// Or with specific ID: registry.spawnEntityWithId(1);
```

## Components

Attach data to entities via Registry.

```cpp
// Emplace
registry.emplaceComponent<Component::position_t>(e, Component::position_t{10.0f, 20.0f});

// Get all
auto &positions = registry.getComponents<Component::position_t>();

// Access
if (positions[e.id()].has_value()) {
    position_t &pos = positions[e.id()].value();
    pos.x += 5;
}

// Iterate
for (size_t i = 0; i < positions.size(); i++) {
    if (positions[i].has_value()) {
        // Do something
    }
}
```

## Systems

Implement `ISystem` interface, update in `update(Registry, dt)`.

Registered in JSON configs, loaded by `SystemManager`.

## Notes

- Systems auto-run added to manager.
- Server-side ECS drives simulation, client receives snapshots.
- Use optional for missing components.
- Registry is shared between systems per side (client/server).
