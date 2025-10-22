/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** InputSystem.cpp
*/

#include "InputSystem.hpp"
#include "MessageType.hpp"

System::InputSystem::InputSystem(std::shared_ptr<Network::network_context_t> ctx)
    : _ctx(std::move(ctx))
{
}

void System::InputSystem::init(Ecs::Registry&)
{
    _keyMapping = {
        {KEY_UP, Network::UP},
        {KEY_DOWN, Network::DOWN},
        {KEY_LEFT, Network::LEFT},
        {KEY_RIGHT, Network::RIGHT},
        {KEY_SPACE, Network::SPACE}
    };
    Logger::info("[InputSystem] Initialized");
}

void System::InputSystem::update(Ecs::Registry&, double)
{
    for (auto &[rayKey, code] : _keyMapping) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            _ctx->quitRequested = true;
        }
        if (IsKeyPressed(rayKey) || IsKeyReleased(rayKey)) {
            Network::Packet pkt;
            pkt.header.type = Network::CLIENT_INPUT;
            pkt.header.seq = 0;
            pkt.payload.push_back(IsKeyPressed(rayKey) ? Network::PRESS : Network::RELEASE);
            pkt.payload.push_back(code);
            if (_ctx->adapter) {
                Network::endpoint_t serverEp;
                serverEp.address = _ctx->serverEndpoint.address().to_string();
                serverEp.port = _ctx->serverEndpoint.port();
                _ctx->adapter->sendReliable(serverEp, pkt);
            }
        }
    }
}

void System::InputSystem::shutdown()
{
    Logger::info("[InputSystem] Shutdown");
}

extern "C" std::shared_ptr<System::ISystem> createInputSystem(std::any params)
{
    std::shared_ptr<Network::network_context_t> ctx;
    try {
        if (params.has_value()) {
            ctx = std::any_cast<std::shared_ptr<Network::network_context_t>>(params);
        }
        return std::make_shared<System::InputSystem>(ctx);
    } catch (const std::exception& e) {
        Logger::error(std::string("[Factory]: Failed to create InputSystem: ") + e.what());
    }
    return nullptr;
}
