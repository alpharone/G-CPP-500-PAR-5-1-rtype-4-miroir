/*
** EPITECH PROJECT, 2025
** repo
** File description:
** RegistryTest
*/

#include <catch2/catch_test_macros.hpp>
#include "Registry.hpp"
#include "SparseArray.hpp"

struct Position {
    int x;
    int y;
};

struct Velocity {
    int vx;
    int vy;
};

TEST_CASE("Registry::spawnEntity produces increasing ids and reuses freed ids", "[Registry][spawn][kill]")
{
    Ecs::Registry reg;

    auto e0 = reg.spawnEntity();
    auto e1 = reg.spawnEntity();
    auto e2 = reg.spawnEntity();

    REQUIRE(static_cast<std::size_t>(e0) == 0u);
    REQUIRE(static_cast<std::size_t>(e1) == 1u);
    REQUIRE(static_cast<std::size_t>(e2) == 2u);

    reg.killEntity(e1);
    auto e3 = reg.spawnEntity();
    REQUIRE(static_cast<std::size_t>(e3) == 1u);

    auto e4 = reg.spawnEntity();
    REQUIRE(static_cast<std::size_t>(e4) == 3u);
}

TEST_CASE("Component registration, emplaceComponent and removeComponent", "[Registry][components]")
{
    Ecs::Registry reg;

    auto e0 = reg.spawnEntity();
    auto e1 = reg.spawnEntity();

    reg.emplaceComponent<Position>(e0, 10, 20);
    reg.emplaceComponent<Position>(e1, 1, 2);

    auto& positions = reg.getComponents<Position>();

    REQUIRE(positions.size() >= 2);
    REQUIRE(positions[static_cast<std::size_t>(e0)].has_value());
    REQUIRE(positions[static_cast<std::size_t>(e0)]->x == 10);
    REQUIRE(positions[static_cast<std::size_t>(e0)]->y == 20);

    REQUIRE(positions[static_cast<std::size_t>(e1)].has_value());
    REQUIRE(positions[static_cast<std::size_t>(e1)]->x == 1);
    REQUIRE(positions[static_cast<std::size_t>(e1)]->y == 2);

    reg.removeComponent<Position>(e0);
    REQUIRE_FALSE(positions[static_cast<std::size_t>(e0)].has_value());
}

TEST_CASE("killEntity triggers erase callbacks that remove components", "[Registry][kill][callbacks]")
{
    Ecs::Registry reg;

    auto e0 = reg.spawnEntity();
    auto e1 = reg.spawnEntity();

    reg.emplaceComponent<Position>(e0, 5, 6);
    reg.emplaceComponent<Velocity>(e0, 1, 1);
    reg.emplaceComponent<Position>(e1, 7, 8);

    auto& pos = reg.getComponents<Position>();
    auto& vel = reg.getComponents<Velocity>();

    REQUIRE(pos[static_cast<std::size_t>(e0)].has_value());
    REQUIRE(vel[static_cast<std::size_t>(e0)].has_value());
    REQUIRE(pos[static_cast<std::size_t>(e1)].has_value());

    reg.killEntity(e0);

    REQUIRE_FALSE(pos[static_cast<std::size_t>(e0)].has_value());
    REQUIRE_FALSE(vel[static_cast<std::size_t>(e0)].has_value());

    REQUIRE(pos[static_cast<std::size_t>(e1)].has_value());
}

TEST_CASE("addSystem and runSystems: system can iterate / modify components", "[Registry][systems]")
{
    Ecs::Registry reg;

    auto e0 = reg.spawnEntity();
    auto e1 = reg.spawnEntity();
    auto e2 = reg.spawnEntity();

    reg.emplaceComponent<Position>(e0, 0, 0);
    reg.emplaceComponent<Position>(e1, 10, 10);

    reg.addSystem<Position>([](Ecs::Registry& r, Ecs::SparseArray<Position>& positions) {
        for (std::size_t i = 0; i < positions.size(); i++) {
            auto& opt = positions[i];
            if (opt.has_value()) {
                opt->x += 1;
            }
        }
    });

    reg.runSystems();

    auto& pos = reg.getComponents<Position>();
    REQUIRE(pos[static_cast<std::size_t>(e0)].has_value());
    REQUIRE(pos[static_cast<std::size_t>(e1)].has_value());

    REQUIRE(pos[static_cast<std::size_t>(e0)]->x == 1);
    REQUIRE(pos[static_cast<std::size_t>(e1)]->x == 11);

    reg.runSystems();
    REQUIRE(pos[static_cast<std::size_t>(e0)]->x == 2);
    REQUIRE(pos[static_cast<std::size_t>(e1)]->x == 12);

    REQUIRE_FALSE(pos[static_cast<std::size_t>(e2)].has_value());
}

TEST_CASE("getComponents const overload throws when component not registered", "[Registry][const][get]")
{
    Ecs::Registry reg;
    auto e0 = reg.spawnEntity();
    reg.emplaceComponent<Position>(e0, 1, 1);

    const Ecs::Registry& creg = reg;
    REQUIRE_NOTHROW((void)creg.getComponents<Position>());

    struct Unregistered { int a; };

    REQUIRE_THROWS_AS((void)creg.getComponents<Unregistered>(), std::out_of_range);
}
