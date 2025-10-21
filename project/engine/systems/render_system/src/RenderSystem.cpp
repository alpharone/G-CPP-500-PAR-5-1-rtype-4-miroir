/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** RenderSystem.cpp
*/

#include "RenderSystem.hpp"
#include <filesystem>

System::RenderSystem::RenderSystem(int width, int height, const std::string& title, std::shared_ptr<Network::network_context_t> ctx)
    : _width(width), _height(height), _title(title), _ctx(std::move(ctx)), _initialized(false)
{}

System::RenderSystem::~RenderSystem()
{
    if (_initialized)
        CloseWindow();
}

void System::RenderSystem::init(Ecs::Registry&)
{
    InitWindow(_width, _height, _title.c_str());
    SetTargetFPS(60);
    _spriteManager = std::make_unique<SpriteManager>();
    _initialized = true;
    Logger::info("[RenderSystem] Initialized (" + std::to_string(_width) + "x" + std::to_string(_height) + ")");
}

void System::RenderSystem::update(Ecs::Registry& registry, double)
{
    if (!_initialized)
        return;
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

            if (itx != drawable.meta.end() && ity != drawable.meta.end()
                && itw != drawable.meta.end() && ith != drawable.meta.end()) {
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
        }
    }

    EndDrawing();
}


void System::RenderSystem::shutdown()
{
    if (_initialized) {
        CloseWindow();
        _initialized = false;
        Logger::info("[RenderSystem] Shutdown");
    }
}

extern "C" std::shared_ptr<System::ISystem> createRenderSystem(std::any params)
{
    int w = 800;
    int h = 600;
    std::string title = "R-Type Client";
    std::shared_ptr<Network::network_context_t> ctx;

    try {
        if (params.has_value()) {
                auto t = std::any_cast<std::tuple<int, int, std::string, std::shared_ptr<Network::network_context_t>>>(params);
                w = std::get<0>(t);
                h = std::get<1>(t);
                title = std::get<2>(t);
                ctx = std::get<3>(t);
        }
        return std::make_shared<System::RenderSystem>(w, h, title, ctx);
    } catch (const std::exception& e) {
        Logger::error(std::string("[Factory]: Failed to create FactorySystem: ") + e.what());
    }
    return nullptr;
}
