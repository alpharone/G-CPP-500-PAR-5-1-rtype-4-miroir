/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** RenderSystem.hpp
*/

#pragma once

#include <raylib.h>

#include <memory>
#include <string>
#include <tuple>

#include "Drawable.hpp"
#include "ISystem.hpp"
#include "Logger.hpp"
#include "NetworkContext.hpp"
#include "Position.hpp"
#include "SpriteManager.hpp"

namespace System {

class RenderSystem : public ISystem {
public:
  RenderSystem(int width, int height, const std::string &title,
               std::shared_ptr<Network::network_context_t> ctx);
  ~RenderSystem() override;

  void init(Ecs::Registry &registry) override;
  void update(Ecs::Registry &registry, double dt) override;
  void shutdown() override;

private:
  int _width;
  int _height;
  std::string _title;
  std::shared_ptr<Network::network_context_t> _ctx;
  std::unique_ptr<SpriteManager> _spriteManager;
  bool _initialized;
};

} // namespace System