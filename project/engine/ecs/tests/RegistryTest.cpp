/*
** EPITECH PROJECT, 2025
** repo
** File description:
** RegistryTest
*/

#include "Registry.hpp"
#include "Entity.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("kill me please", "[Registry]") {
 Ecs::Registry registry;

    auto e1 = registry.spawnEntity();
    auto e2 = registry.spawnEntity();
    SECTION("bro tu es certain d'avoir une id unique ???") {
        REQUIRE(e1 != e2);
    }
    SECTION("bro tu as les bon ids ???") {
        REQUIRE(e1 == 0);
        REQUIRE(e2 == 1);
    }
    SECTION("bro tu te sup bien ???") {
        registry.killEntity(e1);
        auto e3 = registry.spawnEntity();
        REQUIRE(e3 == 0);
    }

}

TEST_CASE("vasy bg de system marche !", "[Registry][System]") {
    Ecs::Registry registry;

    bool systemCalled = false;

    registry.addSystem<>([&](Ecs::Registry& r){
        systemCalled = true;
    });

    registry.runSystems();

    REQUIRE(systemCalled);
}

