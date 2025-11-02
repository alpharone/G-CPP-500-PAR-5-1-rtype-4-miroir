/*
** EPITECH PROJECT, 2025
** repo
** File description:
** EntityTest
*/

#include "Entity.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Ecs::Entity construction and conversion", "[Entity]") {
  SECTION("Construct with small id and convert to size_t") {
    Ecs::Entity e{42u};
    REQUIRE(static_cast<std::size_t>(e) == 42u);
  }

  SECTION("Construct with zero id") {
    Ecs::Entity e{0u};
    REQUIRE(static_cast<std::size_t>(e) == 0u);
  }

  SECTION("Construct with large id") {
    std::size_t large = static_cast<std::size_t>(-1) / 2;
    Ecs::Entity e{large};
    REQUIRE(static_cast<std::size_t>(e) == large);
  }

  SECTION("Constructor and conversion are noexcept") {
    REQUIRE(noexcept(Ecs::Entity(1u)));
    Ecs::Entity e{1u};
    REQUIRE(noexcept(static_cast<std::size_t>(e)));
  }
}

TEST_CASE("Ecs::Entity equality and inequality", "[Entity]") {
  Ecs::Entity a{10u};
  Ecs::Entity b{10u};
  Ecs::Entity c{11u};

  SECTION("Equality is reflexive") { REQUIRE(a == a); }

  SECTION("Equality for same id") { REQUIRE(a == b); }

  SECTION("Inequality for different id") { REQUIRE(a != c); }

  SECTION("== and != are consistent") {
    REQUIRE((a == b) == !(a != b));
    REQUIRE((a == c) == !(a != c));
  }

  SECTION("Transitive-like behavior for equal ids") {
    Ecs::Entity d{10u};
    REQUIRE(a == b);
    REQUIRE(b == d);
    REQUIRE(a == d);
  }
}

TEST_CASE("Ecs::Entity compare edge cases", "[Entity][edge]") {
  SECTION("Different ids always not equal") {
    Ecs::Entity e1{1u};
    Ecs::Entity e2{2u};
    REQUIRE(!(e1 == e2));
    REQUIRE(e1 != e2);
  }

  SECTION("Many Entities with increasing ids") {
    for (std::size_t i = 0; i < 100; i++) {
      Ecs::Entity a{i};
      Ecs::Entity b{i};
      Ecs::Entity c{i + 1};
      REQUIRE(a == b);
      REQUIRE(a != c);
    }
  }
}
