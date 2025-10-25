/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** AnimationSystem.cpp
*/

#include "AnimationSystem.hpp"

#include <any>
#include <sstream>

#include "Animation.hpp"
#include "Drawable.hpp"
#include "Logger.hpp"

System::AnimationSystem::AnimationSystem() {
  _spriteManager = std::make_unique<SpriteManager>();
}

void System::AnimationSystem::init(Ecs::Registry &) {
  Logger::info("[AnimationSystem] init");
}

void System::AnimationSystem::update(Ecs::Registry &registry, double dt) {
  auto &animations = registry.getComponents<Component::animation_t>();
  auto &drawables = registry.getComponents<Component::drawable_t>();

  for (size_t i = 0; i < animations.size() && i < drawables.size(); i++) {
    if (!animations[i].has_value() || !drawables[i].has_value())
      continue;

    auto &anim = animations[i].value();
    auto &drawable = drawables[i].value();

    if (anim.frameW <= 0 || anim.frameH <= 0 || anim.frameCount <= 0) {
      continue;
    }

    anim.elapsed += dt;
    double secPerFrame = 1.0 / static_cast<double>(std::max(anim.fps, 1));
    int framesToAdvance = static_cast<int>(anim.elapsed / secPerFrame);
    if (framesToAdvance > 0) {
      anim.elapsed -= framesToAdvance * secPerFrame;
      anim.currentFrame += framesToAdvance;
      if (anim.loop) {
        if (anim.frameCount > 0)
          anim.currentFrame %= anim.frameCount;
        else
          anim.currentFrame = 0;
      } else {
        if (anim.currentFrame >= anim.frameCount)
          anim.currentFrame = anim.frameCount - 1;
      }
    }

    int fx = anim.startX + (anim.currentFrame * anim.frameW);
    int fy = anim.startY;
    int fw = anim.frameW;
    int fh = anim.frameH;

    drawable.meta["frame_x"] = std::to_string(fx);
    drawable.meta["frame_y"] = std::to_string(fy);
    drawable.meta["frame_w"] = std::to_string(fw);
    drawable.meta["frame_h"] = std::to_string(fh);
  }
}

void System::AnimationSystem::shutdown() {
  Logger::info("[AnimationSystem] shutdown");
  if (_spriteManager)
    _spriteManager->unloadAll();
}

extern "C" std::shared_ptr<System::ISystem> createAnimationSystem(std::any) {
  try {
    return std::make_shared<System::AnimationSystem>();
  } catch (const std::exception &e) {
    Logger::error(std::string("[Factory] Failed to create AnimationSystem: ") +
                  e.what());
  }
  return nullptr;
}
