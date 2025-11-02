/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** PositionSystem
*/

#pragma once

#include "ISystem.hpp"

namespace System {

class PositionSystem : public ISystem {
public:
  PositionSystem();
  ~PositionSystem() override = default;

  void init(Ecs::Registry &registry) override;
  void update(Ecs::Registry &registry, double dt) override;
  void shutdown() override;
};

} // namespace System
