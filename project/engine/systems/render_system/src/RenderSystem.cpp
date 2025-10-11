/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** RenderSystem.cpp
*/

#include <tuple>
#include <thread>
#include "RenderSystem.hpp"
#include "Logger.hpp"
#include "Drawable.hpp"

RenderSystem::RenderSystem(int w, int h, const std::string& title, std::shared_ptr<NetworkContext> ctx) : _w(w), _h(h), _title(title), _ctx(std::move(ctx))
{
    if (_title.empty())
        _title = "R-Type Client";

    InitWindow(_w, _h, _title.c_str());
    SetTargetFPS(60);
    Logger::info("[Render] Window initialized: " + _title);
}

RenderSystem::~RenderSystem()
{
    if (IsWindowReady()) {
        _sprites.unloadAll();
        CloseWindow();
    }
}

void RenderSystem::update(Ecs::Registry& registry, float)
{
    if (!IsWindowReady())
        return;

    PollInputEvents();
    BeginDrawing();
    ClearBackground(BLACK);

    auto& positions = registry.getComponents<Component::position_t>();
    auto& drawables = registry.getComponents<Component::drawable_t>();
    size_t count = std::min(positions.size(), drawables.size());

    std::vector<size_t> entities;
    entities.reserve(count);
    for (size_t i = 0; i < count; i++)
        if (positions[i].has_value() && drawables[i].has_value())
            entities.push_back(i);

    std::stable_sort(entities.begin(), entities.end(), [&](size_t a, size_t b) {
        return drawables[a]->z < drawables[b]->z;
    });

    for (size_t id : entities) {
        const auto& pos = positions[id].value();
        auto& draw = const_cast<Component::drawable_t&>(drawables[id].value());

        Color tint = draw.color;
        bool isLocal = (_ctx && _ctx->playerId == static_cast<uint32_t>(id));

        if (!draw.texturePath.empty() && !draw.loaded) {
            draw.texture = _sprites.load(draw.texturePath);
            draw.loaded = (draw.texture.id != 0);
            if (draw.loaded)
                Logger::info("[Render] Loaded texture: " + draw.texturePath);
        }

        if (draw.loaded && draw.texture.id != 0) {
            float x = pos.x - draw.texture.width / 2.0f;
            float y = pos.y - draw.texture.height / 2.0f;
            DrawTexture(draw.texture, static_cast<int>(x), static_cast<int>(y), tint);

            if (isLocal)
                DrawRectangleLines(static_cast<int>(x) - 2, static_cast<int>(y) - 2,
                                   draw.texture.width + 4, draw.texture.height + 4, WHITE);
        } else {
            DrawCircle(static_cast<int>(pos.x), static_cast<int>(pos.y), 12.f, tint);
            if (isLocal)
                DrawCircleLines(static_cast<int>(pos.x), static_cast<int>(pos.y), 15.f, WHITE);
        }
    }

    EndDrawing();
}

extern "C" std::shared_ptr<ISystem> createRenderSystem(std::any params)
{
    int w = 800, h = 600;
    std::string title = "R-Type Client";
    std::shared_ptr<NetworkContext> ctx;

    if (params.has_value()) {
        try {
            auto t = std::any_cast<std::tuple<int, int, std::string, std::shared_ptr<NetworkContext>>>(params);
            w = std::get<0>(t);
            h = std::get<1>(t);
            title = std::get<2>(t);
            ctx = std::get<3>(t);
        } catch (...) {
            Logger::warn("[RenderFactory] bad any_cast");
        }
    }

    return std::make_shared<RenderSystem>(w, h, title, ctx);
}