/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** AnimationSystem.hpp
*/

#pragma once

#include "ISystem.hpp"
#include "SpriteManager.hpp"
#include <any>
#include <memory>
#include <string>
#include <vector>

namespace System {

class AnimationSystem : public ISystem {
public:
  AnimationSystem();
  ~AnimationSystem() override = default;

  void init(Ecs::Registry &registry) override;
  void update(Ecs::Registry &registry, double dt) override;
  void shutdown() override;

  void applyAnimation(Ecs::Registry &registry, Ecs::Entity entity, int frameW,
                      int frameH, int frameCount, int fps = 12,
                      bool loop = true, int startX = 0, int startY = 0,
                      const std::string &name = "");

private:
  std::unique_ptr<SpriteManager> _spriteManager;
};

} // namespace System
