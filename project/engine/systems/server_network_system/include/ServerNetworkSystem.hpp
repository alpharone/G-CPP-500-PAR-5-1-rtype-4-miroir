/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ServerNetworkSystem
*/

#pragma once

#include <asio.hpp>
#include <unordered_map>
#include <string>
#include <chrono>
#include "Packets.hpp"
#include "ReliableChannel.hpp"
#include "GameInstance.hpp"
#include "ISystem.hpp"
#include "Logger.hpp"
#include "Room.hpp"

using asio::ip::udp;

class ServerNetworkSystem : public ISystem {
    public:
        explicit ServerNetworkSystem(unsigned short port);
        void update(Ecs::Registry& registry, float dt) override;

    private:
        void startReceive();
        void onReceive(const udp::endpoint& from, const uint8_t* data, size_t len);
        void spawnPlayer(Room& room, uint32_t localId, const udp::endpoint& endpoint, Ecs::Registry& registry);
        void applyInputs(Room& room, float dt, Ecs::Registry& registry);
        void sendSnapshotToRoom(Room& room, Ecs::Registry& registry);
        void broadcastDespawn(Room& room, uint32_t globalId);

        asio::io_context _io;
        udp::socket _socket;
        udp::endpoint _remoteEndpoint;
        std::array<uint8_t, 1500> _buffer{};

        std::vector<std::unique_ptr<Room>> _rooms;
        std::mutex _inputMutex;
};