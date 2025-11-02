/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** PositionSystem.cpp
*/

#include "PositionSystem.hpp"
#include "Position.hpp"
#include "Velocity.hpp"

System::PositionSystem::PositionSystem() = default;

void System::PositionSystem::init(Ecs::Registry &) {}

void System::PositionSystem::update(Ecs::Registry &registry, double dt) {
  auto &positions = registry.getComponents<Component::position_t>();
  auto &velocities = registry.getComponents<Component::velocity_t>();

  for (size_t i = 0; i < positions.size() && i < velocities.size(); ++i) {
    if (!positions[i].has_value() || !velocities[i].has_value()) {
      continue;
    }

    auto &pos = positions[i].value();
    auto &vel = velocities[i].value();

    pos.x += vel.vx * dt;
    pos.y += vel.vy * dt;
  }
}

void System::PositionSystem::shutdown() {}

extern "C" std::shared_ptr<System::ISystem>
createPositionSystem(std::any params) {
  return std::make_shared<System::PositionSystem>();
}
