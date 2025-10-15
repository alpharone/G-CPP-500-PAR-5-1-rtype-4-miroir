/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ServerNetworkSystem
*/

#pragma once

#include <asio.hpp>
#include <array>
#include <deque>
#include <vector>
#include <memory>
#include <any>
#include "ISystem.hpp"
#include "Logger.hpp"
#include "Registry.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "NetworkContext.hpp"
#include "Packets.hpp"

using asio::ip::udp;

class ClientNetworkSystem : public ISystem, public std::enable_shared_from_this<ClientNetworkSystem> {
    public:
        ClientNetworkSystem(const std::string& host,
                            unsigned short port,
                            std::shared_ptr<NetworkContext> ctx);
        ~ClientNetworkSystem() override;
        
        void start();
        void update(Ecs::Registry& registry, float dt) override;
        void shutdown() override;
        
    private:
        void startReceive();
        void sendJoinRequest();
        void onReceiveData(const uint8_t* data, size_t len);
        void handleEntitySpawn(const Network::Packet& pkt, Ecs::Registry& registry);
        void handleSnapshot(const Network::Packet& pkt, Ecs::Registry& registry);
        void handleEntityDespawn(const Network::Packet& pkt, Ecs::Registry& registry);
        void sendDisconnect();
        void stopIo();

        asio::io_context _io;
        std::shared_ptr<udp::socket> _socket;
        udp::endpoint _serverEndpoint;
        udp::endpoint _senderEndpoint;
        std::array<uint8_t, 1500> _buffer{};
        
        std::shared_ptr<NetworkContext> _ctx;
        std::atomic_bool _running{false};
        std::thread _ioThread;
        
        std::deque<Network::Packet> _snapQueue;
        std::mutex _queueMutex;
        
        std::optional<uint32_t> _playerId;
        uint64_t _lastPingTs = 0;
};
