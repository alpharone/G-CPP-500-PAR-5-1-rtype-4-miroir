# Developer Documentation

This documentation provides guides and explanations for developers working on the R-Type game engine project.

## Table of Contents

- [ECS Usage](ecsUsage.md) - Understanding the Entity-Component-System architecture
- [Adding Components](addingComponents.md) - How to define and use new components
- [Adding Systems](addingSystems.md) - Implementing and integrating new systems
- [Networking](networking.md) - Using the network protocol and systems
- [Configuration](configuration.md) - Managing game settings via JSON configs

## Overview

The engine is built using an ECS (Entity-Component-System) architecture, allowing for modular and extensible game logic.

Systems are loaded dynamically as plugins from shared libraries, enabling hot-replacement and modularity.

Networking is handled via reliable UDP with Asio, supporting both client and server operations.

For any questions, refer to the code comments or create an issue in the repository.
