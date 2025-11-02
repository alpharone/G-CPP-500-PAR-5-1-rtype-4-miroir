# Adding Components

Components define the data attached to entities. This guide explains how to define new components for the ECS.

## Step 1: Define Component Struct

Create `project/engine/components/include/YourComponent.hpp`:

```cpp
#pragma once

#include <cstdint>
#include <string>

namespace Component {

struct yourcomponent_t {
    int value = 0;
    std::string tag = "default";
};

}
```

- Use structs for components.
- Use meaningful names ending with `_t`.

## Step 2: Use in Code

In systems or game logic:

```cpp
// Get components
auto &comps = registry.getComponents<Component::yourcomponent_t>();

// Add to entity
registry.emplaceComponent<Component::yourcomponent_t>(entity, Component::yourcomponent_t{42, "test"});

// Use
for (size_t i = 0; i < comps.size(); i++) {
    if (comps[i].has_value()) {
        yourcomponent_t &comp = comps[i].value();
        // Logic here
    }
}
```

## Notes

- Components are automatically managed by Registry.
- Avoid large data; prefer aggregates or pointers if needed.
- For network sync, add serialization if sending over network.
- Example: `velocity_t` adds width/height for collision sizes.
