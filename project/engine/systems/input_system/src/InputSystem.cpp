/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** InputSystem.cpp
*/

#include <tuple>
#include "InputSystem.hpp"

InputSystem::InputSystem(std::shared_ptr<NetworkContext> ctx) : _ctx(std::move(ctx))
{
    Logger::info("[InputSystem] Initialized");
}

void InputSystem::update(Ecs::Registry&, float)
{
    if (!_ctx || !_ctx->socket || !_ctx->socket->is_open())
        return;

    float dx = 0.f;
    float dy = 0.f;

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        dy -= 1.f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        dy += 1.f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        dx -= 1.f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        dx += 1.f;

    if (dx == 0.f && dy == 0.f)
        return;

    float len = std::sqrt(dx*dx + dy*dy);
    if (len > 0.f) {
        dx /= len;
        dy /= len;
    }

    Network::Packet pkt;
    pkt.header.type = Network::PLAYER_INPUT;
    pkt.payload.resize(2 * sizeof(float));
    std::memcpy(pkt.payload.data(), &dx, sizeof(float));
    std::memcpy(pkt.payload.data()+sizeof(float), &dy, sizeof(float));

    try {
        _ctx->socket->send_to(asio::buffer(pkt.serialize()), _ctx->serverEndpoint);
    } catch (const std::exception& e) {
        Logger::warn(std::string("[InputSystem] send failed: ") + e.what());
    }
}

extern "C" std::shared_ptr<ISystem> createInputSystem(std::any params)
{
    std::shared_ptr<NetworkContext> ctx;

    if (params.has_value()) {
        try {
            ctx = std::any_cast<std::shared_ptr<NetworkContext>>(params);
        }
        catch (const std::exception& e) {
            Logger::warn(std::string("[InputFactory] bad any_cast: ") + e.what());
        }
    }
    return std::make_shared<InputSystem>(ctx);
}
