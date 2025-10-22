/*
** EPITECH PROJECT, 2025
** repo
** File description:
** NetworkContext
*/

#pragma once

#include <asio.hpp>
#include <memory>
#include <queue>
#include "Packets.hpp"
#include "ReliableLayerAdapter.hpp"
#include "Registry.hpp"

namespace Network {

    struct network_context_t {
        std::shared_ptr<asio::ip::udp::socket> socket;
        asio::ip::udp::endpoint serverEndpoint;
        uint32_t playerId = UINT32_MAX;
        Ecs::Registry* registry;

        std::shared_ptr<ReliableLayerAdapter> adapter;

        std::mutex queueMutex;
        std::queue<Network::Packet> incoming;

        std::atomic_bool connected{false};
        bool quitRequested = false;

        uint32_t roomId = 0;
        uint32_t clientId = 0;

        void pushPacket(const Network::Packet& pkt);
        bool popPacket(Network::Packet& out);
    };

}
