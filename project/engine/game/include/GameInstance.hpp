/*
** EPITECH PROJECT, 2025
** repo
** File description:
** GameInstance
*/

#pragma once
#include <unordered_map>
#include <queue>
#include <memory>
#include <vector>
#include <cstdint>
#include <asio.hpp>
#include "Registry.hpp"
#include "ISystem.hpp"
#include "SystemManager.hpp"
#include "Packets.hpp"
#include "Position.hpp"
#include "Logger.hpp"
// #include "Map.hpp"
// #include "Connection.hpp"

namespace System {

class GameInstance : public ISystem {
public:
    GameInstance();
    ~GameInstance() = default;

    //void tick();
    //void addPlayer(Connection& conn, uint32_t clientId);
    //void removePlayer(uint32_t clientId);
    void init(Ecs::Registry& registry) {};
    void update(Ecs::Registry& registry, double dt) {};
    void shutdown() {};
    void applyInput(uint32_t clientId, const std::vector<uint8_t>& input);
    Network::Packet produceSnapshot();

private:
    std::shared_ptr<Ecs::Registry> _registry;
    SystemManager _systemManager;
   // Map _mapData;
    uint32_t _tickRate = 60;
    //std::unordered_map<uint32_t, Connection> _players;
    //std::queue<GameEvent> _eventQueue;
};
}