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

    struct DrawEntry {
        size_t id;
        Component::position_t pos;
        Component::drawable_t draw;
    };

    std::vector<DrawEntry> entries;
    entries.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        if (positions[i].has_value() && drawables[i].has_value()) {
            entries.push_back({i, positions[i].value(), drawables[i].value()});
        }
    }

    std::stable_sort(entries.begin(), entries.end(),
                     [](const DrawEntry& a, const DrawEntry& b) {
                         return a.draw.z < b.draw.z;
                     });

    for (auto& e : entries) {
        const auto& p = e.pos;
        const auto& d = e.draw;

        Color tint = d.color;
        bool isLocal = (_ctx && _ctx->playerId == static_cast<uint32_t>(e.id));

        if (!d.texturePath.empty()) {
            Texture2D tex = _sprites.load(d.texturePath);
            if (tex.id != 0) {
                float x = p.x - tex.width / 2.0f;
                float y = p.y - tex.height / 2.0f;
                DrawTexture(tex, static_cast<int>(x), static_cast<int>(y), tint);

                if (isLocal)
                    DrawRectangleLines(static_cast<int>(x) - 2, static_cast<int>(y) - 2,
                                       tex.width + 4, tex.height + 4, WHITE);
                continue;
            }
        }

        DrawCircle(static_cast<int>(p.x), static_cast<int>(p.y), 12.f, tint);
        if (isLocal)
            DrawCircleLines(static_cast<int>(p.x), static_cast<int>(p.y), 15.f, WHITE);
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