/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** RenderSystem.cpp
*/

#include "RenderSystem.hpp"
#include <filesystem>

System::RenderSystem::RenderSystem(
    int width, int height, const std::string &title,
    std::shared_ptr<Network::network_context_t> ctx)
    : _width(width), _height(height), _title(title), _ctx(std::move(ctx)),
      _initialized(false) {}

System::RenderSystem::~RenderSystem() {
  if (_initialized)
    CloseWindow();
}

void System::RenderSystem::init(Ecs::Registry &) {
  InitWindow(_width, _height, _title.c_str());
  SetTargetFPS(60);
  _spriteManager = std::make_unique<SpriteManager>();
  _initialized = true;
  Logger::info("[RenderSystem] Initialized (" + std::to_string(_width) + "x" +
               std::to_string(_height) + ")");
}

void System::RenderSystem::update(Ecs::Registry &registry, double) {
  if (!_initialized)
    return;
  if (WindowShouldClose()) {
    if (_ctx)
      _ctx->quitRequested = true;
    return;
  }
  BeginDrawing();
  ClearBackground(BLACK);

  auto &positions = registry.getComponents<Component::position_t>();
  auto &drawables = registry.getComponents<Component::drawable_t>();

  for (size_t i = 0; i < positions.size() && i < drawables.size(); i++) {
    if (!positions[i].has_value() || !drawables[i].has_value())
      continue;
    const auto &pos = positions[i].value();
    const auto &drawable = drawables[i].value();
    Texture2D texture = _spriteManager->getTexture(drawable.renderType);
    if (texture.id != 0) {
      auto itx = drawable.meta.find("frame_x");
      auto ity = drawable.meta.find("frame_y");
      auto itw = drawable.meta.find("frame_w");
      auto ith = drawable.meta.find("frame_h");

      if (itx != drawable.meta.end() && ity != drawable.meta.end() &&
          itw != drawable.meta.end() && ith != drawable.meta.end()) {
        int fx = std::stoi(itx->second);
        int fy = std::stoi(ity->second);
        int fw = std::stoi(itw->second);
        int fh = std::stoi(ith->second);
        Rectangle src = {(float)fx, (float)fy, (float)fw, (float)fh};
        Vector2 dst = {(float)pos.x, (float)pos.y};
        DrawTextureRec(texture, src, dst, WHITE);
      } else {
        DrawTexture(texture, (int)pos.x, (int)pos.y, WHITE);
      }
    } else {
      Logger::warn("[Render] Texture not loaded for " + drawable.renderType);
    }
  }

  EndDrawing();
}

void System::RenderSystem::shutdown() {
  if (_initialized) {
    CloseWindow();
    _initialized = false;
    Logger::info("[RenderSystem] Shutdown");
  }
}

extern "C" std::shared_ptr<System::ISystem>
createRenderSystem(std::any params) {
  try {
    auto vec = std::any_cast<std::vector<std::any>>(params);
    int width = std::any_cast<int>(vec[0]);
    int height = std::any_cast<int>(vec[1]);
    std::string title = std::any_cast<std::string>(vec[2]);
    auto ctx =
        std::any_cast<std::shared_ptr<Network::network_context_t>>(vec[3]);
    return std::make_shared<System::RenderSystem>(width, height, title, ctx);
  } catch (const std::exception &e) {
    Logger::error(std::string("[Factory]: Failed to create RenderSystem: ") +
                  e.what());
  }
  return nullptr;
}
