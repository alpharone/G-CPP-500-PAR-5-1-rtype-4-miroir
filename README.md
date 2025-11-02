# R-Type

R-Type is a multiplayer remake of the legendary 90s side-scrolling shoot’em up, designed as part of an advanced C++ project. This version is fully networked, features a custom-built game engine, and adheres to modern software engineering practices. Up to four players can join forces in real-time to battle the infamous Bydos in a synchronized, action-packed space battlefield.

## Objectives

- Implement a multi-threaded server and a graphical client.
- Design a modular, scalable game engine from scratch.
- Explore advanced topics: reliable networking, real-time rendering, and gameplay systems.
- Support cross-platform play between Linux and Windows.

## Technologies

| Purpose             | Technology |
|---------------------|------------|
| Language            | C++        |
| Build System        | CMake      |
| Dependency Manager  | CPM.cmake  |
| Networking          | Asio       |
| Rendering / Audio   | Raylib     |
| Units tests         | Catch2     |

[A comparative study was conducted here.](/docs/ComparativeStudy.pdf)

## Build and Run

### Clone the Repository

```bash
> git clone git@github.com:EpitechPGE3-2025/G-CPP-500-PAR-5-1-rtype-4.git r-type
> cd r-type
```

### Build

```bash
> cmake -S . -DBUILD_TESTS=ON -DBUILD_PLUGINS=ON
> make -j
```

### Run

#### Server

```bash
./r-type_server
```

#### Client

```bash
./r-type_client
```

## Project Structure

```
r-type/
├── Dockerfile
├── README.md
├── assets/
├── cmake/
│   └── CPM.cmake
├── configfiles/
├── docker-compose.yml
├── docs/
│   ├── architecture.plantuml
│   ├── gameSequence.plantuml
│   └── protocol.txt
├── logs/
└── project/
    ├── client/
    ├── engine/
    │   ├── components/
    │   ├── ecs/
    │   ├── game/
    │   ├── network/
    │   ├── systems/
    │   │   ├── ai_system/
    │   │   ├── client_network_system/
    │   │   ├── collision_system/
    │   │   ├── control_system/
    │   │   ├── include/
    │   │   ├── src/
    │   │   └── ...
    │   └── tools/
    └── server/
```

## Developer Documentation

Comprehensive guides for developers to extend and maintain the engine are available in the [docs/dev/](docs/dev/) directory.

This includes tutorials on:
- Adding new systems and components
- Understanding the ECS architecture
- Configuring the engine via JSON
- Extending the networking protocol

Start with the [developers' guide index](docs/dev/index.md) for navigation.

## Network Protocol

* Transport: UDP
* All messages use a binary format
* Includes: NEW_CLIENT, ACCEPT_CLIENT, CLIENT_INPUT, SERVER_SNAPSHOT, etc.
* [Full protocol spec available here](docs/protocol.txt)

## Authors

* [Tiphaine PAUTONNIER](https://github.com/tframboise)
* [Alexandre ODRISOLO](https://github.com/alpharone)
* [Nathan CANCEL](https://github.com/Nathcaa)
