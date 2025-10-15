/*
** EPITECH PROJECT, 2025
** repo
** File description:
** EntityTest
*/

#include <catch2/catch_test_macros.hpp>
#include "Entity.hpp"

TEST_CASE("Entity basic behavior", "[Entity]") {
    Ecs::Entity e1(42);
    Ecs::Entity e2(42);
    Ecs::Entity e3(7);

    SECTION("Conversion to size_t") {
        REQUIRE(static_cast<size_t>(e1) == 42);
    }

    SECTION("Equality operator") {
        REQUIRE(e1 == e2);
        REQUIRE_FALSE(e1 == e3);
    }

    SECTION("Inequality operator") {
        REQUIRE(e1 != e3);
        REQUIRE_FALSE(e1 != e2);
    }
}
