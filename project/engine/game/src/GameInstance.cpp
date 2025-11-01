/*
** EPITECH PROJECT, 2025
** repo
** File description:
** GameInstance
*/

#include "GameInstance.hpp"
// #include "CollisionSystem.hpp"
#include "MessageType.hpp"
#include "Packets.hpp"
#include <cstring>
#include <iostream>

System::GameInstance::GameInstance()
{
    _registry = std::make_shared<Ecs::Registry>();
}

// void System::GameInstance::tick()
// {
//     float dt = 1.f / static_cast<float>(_tickRate);

//     for (auto& [id, player] : _players) {
//         applyInput(id, player.currentInput);
//     }

//     _systemManager.updateAll(*_registry, dt);

//     static float mobTimer = 0.f;
//     mobTimer += dt;
//     if (mobTimer >= 3.f) {
//         mobTimer = 0.f;
// //app le systeme de création de monstre si je garde le if
//     }

//     while (!_eventQueue.empty()) {
//         auto evt = _eventQueue.front();
//         _eventQueue.pop();
//     }
//     Network::Packet snapshot = produceSnapshot();
//     for (auto& [id, player] : _players) {
//         if (!player.ready) continue;
//         player.pendingPackets.push(snapshot);
//     }
// }

// void System::GameInstance::addPlayer(Connection& conn, uint32_t clientId)
// {
//     conn.clientId = clientId;
//     conn.ready = true;
//     _players[clientId] = conn;
// }

// void System::GameInstance::removePlayer(uint32_t clientId)
// {
//     _players.erase(clientId);
// }

 void System::GameInstance::applyInput(uint32_t clientId, const std::vector<uint8_t>& input)
{
    auto& posArray = _registry->getComponents<Component::position_t>();
    auto& velArray = _registry->getComponents<Component::position_t>();

    if (clientId >= posArray.size()) return;
    auto& pos = posArray[clientId];
    auto& vel = velArray[clientId];

    if (!pos.has_value() || !vel.has_value()) return;

    vel->x = static_cast<float>(input[0]) * 100.f;
    vel->y = static_cast<float>(input[1]) * 100.f;
}

Network::Packet System::GameInstance::produceSnapshot()
{
    Network::Packet pkt;
    pkt.header.type = Network::SERVER_SNAPSHOT;

    auto& posArray = _registry->getComponents<Component::position_t>();
    auto& velArray = _registry->getComponents<Component::position_t>();

    uint16_t count = 0;
    pkt.payload.resize(sizeof(uint16_t));
    size_t offset = sizeof(uint16_t);

    for (size_t i = 0; i < posArray.size(); ++i) {
        if (!posArray[i].has_value() || !velArray[i].has_value())
            continue;

        float x = posArray[i]->x;
        float y = posArray[i]->y;
        float vx = velArray[i]->x;
        float vy = velArray[i]->y;
        uint32_t id = static_cast<uint32_t>(i);

        pkt.payload.resize(offset + sizeof(uint32_t) + 4 * sizeof(float));
        std::memcpy(pkt.payload.data() + offset, &id, sizeof(uint32_t)); offset += sizeof(uint32_t);
        std::memcpy(pkt.payload.data() + offset, &x, sizeof(float)); offset += sizeof(float);
        std::memcpy(pkt.payload.data() + offset, &y, sizeof(float)); offset += sizeof(float);
        std::memcpy(pkt.payload.data() + offset, &vx, sizeof(float)); offset += sizeof(float);
        std::memcpy(pkt.payload.data() + offset, &vy, sizeof(float)); offset += sizeof(float);

        ++count;
    }

    std::memcpy(pkt.payload.data(), &count, sizeof(uint16_t));
    return pkt;
}

extern "C" std::shared_ptr<System::ISystem> createInputSystem(std::any params)
{
    try {
        return std::make_shared<System::GameInstance>();
    } catch (const std::exception& e) {
        Logger::error(std::string("[Factory]: Failed to create InputSystem: ") + e.what());
    }
    return nullptr;
}